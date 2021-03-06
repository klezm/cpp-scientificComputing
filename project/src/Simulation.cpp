#include <tgmath.h>
#include <iostream>
#include <algorithm>

#include "../include/Simulation.h"
#include "../include/Constants.h"
#include "../helper/include/IOManager.h"
#include "../graphics/graphic.h"

LH::Simulation::Simulation ()
#ifdef GRAPHICS
    : _graphic ( 1000, 1000, 500 ) // window width and height as well as the target framerate
#endif

{
#ifdef GRAPHICS
    // initialize camera
    _graphic.set_viewing_pos( ASTEROID_POS_START );
    _graphic.set_viewing_scale( 30.0f );
#endif

    // initialize the planet
    _planet = new LH::Body;

    _planet->pos      = PLANET_POS;
    _planet->mass     = PLANET_MASS;
    _planet->radius   = PLANET_RADIUS_START;
    _planet->vel      = PLANET_VEL_START;

    _massPoints.push_back( _planet );

    // adding a cloud of masspoints representing the asteroid to _masspoints
    buildSpiral ();

    // seed the RNG
    srand( time(NULL) );
}

void LH::Simulation::run ()
{
    // setup the view
   _view = _massPoints[1];

    LH::IOManager ioDat ( DAT_FILE );

#ifdef POS_DATA
    LH::IOManager ioPos ( POS_FILE );
    ioPos << "# planetX\tplanetY";
    for ( int i = 0; i < MASSPOINTS_NUM; ++ i )
    {
        ioPos << "\tbodyX_";
        ioPos << i;
        ioPos << "\tbodyY_";
        ioPos << i;
    }
    ioPos << "\n";
#endif
    char c[100];

    int J = 0;

#ifdef GRAPHICS
    // main loop (w/o maxtime if using graphics)
    for ( long double t = 0; 1; t += TIME_STEP )
#endif
#ifdef POS_DATA
    for ( long double t = 0; t < MAX_TIME; t += TIME_STEP )
#endif
    {
        int i = 0;

        for ( LH::Body* o : _massPoints )
        {
            for (LH::Body *v : _massPoints)
            {
                if (o == v)
                {
                    continue;
                }
                // calculate gravity
                LH::Vector force = gravity(o, v);
                // apply acceleration
                o->vel += TIME_STEP * (force / o->mass);
            }
            // apply velocity
            o->pos += TIME_STEP * o->vel;
#ifdef POS_DATA
                snprintf(c, sizeof(c), "\t%Lf\t%Lf", _massPoints[i]->pos.x, _massPoints[i]->pos.y);
                ioPos << c;
                i++;
         }
        ioPos << "\n";
#endif
#ifndef POS_DATA
        i++;
        }
#endif

        // might help to minimize rotation of the asteroid
        std::random_shuffle( _massPoints.begin() + 1, _massPoints.end() );

        // collision passes
        for ( int j = 0; j < COLLISION_PASSES; ++j )
        {
            for ( LH::Body* o : _massPoints )
            {
                for ( LH::Body* v : _massPoints )
                {
                    if ( o == v )
                    {
                        continue;
                    }
                    collision( o, v );
                }
            }
        }

        // prints information about momentum, energy and distances for for every OUTPUT_FRAMES (=200) steps
        if ( J++ == OUTPUT_FRAMES )
        {
            long double total_energy = 0.0;
            LH::Vector total_momentum = _massPoints[0]->mass * _massPoints[0]->vel;
            long double av_distance = 0.0;
            long double dist_to_planet = 0.0;
            LH::Vector av_position;
            for ( int j = 1; j < _massPoints.size(); ++j )
            {
                LH::Body* body = _massPoints[j];

                total_energy += 0.5 * body->mass * pow(body->vel.size(),2) - GRAVITY_CONSTANT * body->mass * _massPoints[0]->mass / (body->pos - _massPoints[0]->pos).size();
                av_position += body->pos;
                total_momentum += body->mass * body->vel;
                for ( int k = 1; k < _massPoints.size(); ++k )
                {
                    av_distance += (body->pos - _massPoints[k]->pos).size();
                }
            }
            av_position = av_position / MASSPOINTS_NUM;
            dist_to_planet = (_massPoints[0]->pos - av_position).size();
            printf( "Momentum: %Lf; Average distance: %Lf; Distance to planet: %Lf; Energy: %.5Lf\n", total_momentum.size(), av_distance / MASSPOINTS_NUM, dist_to_planet
                    , total_energy );
            snprintf( c, sizeof( c ), "%Lf\t%Lf\t%LF\t%Lf\t%Lf\n", t, dist_to_planet, av_distance / MASSPOINTS_NUM, total_momentum.size(), total_energy );
            ioDat << c;
            J = 0;
        }

#ifdef GRAPHICS
        make_graphics();

        // quit if user closed window
        if ( _graphic.get_state() == State::QUIT )
        {
            break;
        }
#endif
    }
}

#ifdef GRAPHICS
void LH::Simulation::make_graphics ()
{
    // vector to give pos, radius and path to texture/image file
    std::vector<std::tuple<LH::Vector,float,std::string>> sprites;

    for ( auto&& m : _massPoints )
    {
        sprites.emplace_back( m->pos, m->radius, "./graphics/black-circle.png" );
    }

    // set active sprites
    _graphic.set_sprites( sprites );
    _graphic.set_viewing_pos( _view->pos );

    // actual drawing
    _graphic.draw();
}
#endif

LH::Vector LH::Simulation::gravity ( const LH::Body* A, const LH::Body* B ) const
{
    LH::Vector dist = B->pos - A->pos;
    return GRAVITY_CONSTANT * ((A->mass * B->mass) / pow( dist.size(), 2 )) * dist.unit();
}

void LH::Simulation::collision ( LH::Body* A, LH::Body* B )
{
    LH::Vector dist = B->pos - A->pos;
    long double diff = B->radius + A->radius - dist.size();
    if ( diff < 0 )
    {
        return;
    }

    LH::Vector pos_response = dist.unit() * diff;
    // resolve collision
    A->pos -= 0.5 * pos_response;
    B->pos += 0.5 * pos_response;
    // calculate relative velocity along dist
    LH::Vector vel_response = dist.unit() * (A->vel - B->vel).dot_product( dist.unit() );
    // apply relative velocity on both objects (they have the same mass) to achieve an inelastic collision
    A->vel -= B->mass / ( A->mass + B->mass ) * vel_response;
    B->vel += A->mass / ( A->mass + B->mass ) * vel_response;
}

LH::Simulation::~Simulation ()
{
    for ( auto o : _massPoints )
    {
        delete o;
    }
}

void LH::Simulation::buildSpiral ()
{
    // polar coordinates
    long double phi = 3.;
    long double r;
    // cartesian coordinates
    long double x;
    long double y;

    // factor for calculating the archimedic spiral
    long double a = .321660216662 * MASSPOINTS_RADIUS;

    long double pi = atan ( 1 ) * 4;

    // looping over all bodyCloud elements and adding them to a vector
    for ( int i = 0; i < MASSPOINTS_NUM; ++ i )
    {
        LH::Body *bodyElementB = new LH::Body;

        // formula for archimedic spiral (polar + cartesian)
        r = a * phi;
        x = r * cos( phi );
        y = r * sin( phi );

        //creating a bodyCloud element
        bodyElementB->mass = MASSPOINTS_MASS;
        bodyElementB->radius = MASSPOINTS_RADIUS;
        bodyElementB->vel = ASTEROID_VEL_START;
        bodyElementB->pos = LH::Vector ( x, y ) + ASTEROID_POS_START;

        // writing the elements into a vector
        _massPoints.push_back ( bodyElementB );

        long double delta = 1 / r;

        // increment phi for the archimedic spiral
        phi += delta;
    }
}
//==================================================================
/// DrawPrims.h
///
/// Created by Davide Pasca - 2017/3/28
///
//==================================================================

#ifndef DRAWPRIMS_H
#define DRAWPRIMS_H

//==================================================================
inline void DP_DrawSphere(
                double r,
                int longs,
                int lats,
                const float *pPosOff=nullptr )
{
    if ( pPosOff )
    {
        glPushMatrix();
        glTranslatef( pPosOff[0], pPosOff[1], pPosOff[2] );
    }

    auto output_vertex = [=](int lat, int lon)
    {
        //static const double pi = 3.14159265358979323846;
        auto u = (float)lon / (float)longs;
        auto v = (float)lat / (float)lats;

        glTexCoord2f( u, v );

        auto lo = (float)(2.0*M_PI) * u;
        auto la = (float)(1.0*M_PI) * v;

        float vec[3] {
            cos(lo)*sin(la),
            sin(lo)*sin(la),
            cos(la),
        };

        glNormal3fv(vec);

        vec[0] *= (float)r;
        vec[1] *= (float)r;
        vec[2] *= (float)r;
        glVertex3fv(vec);
    };

    for (int lat=0; lat < lats; ++lat)
    {
        glBegin(GL_TRIANGLE_STRIP);
        for (int lon=0; lon <= longs; ++lon)
        {
            output_vertex(lat, lon);
            output_vertex(lat+1, lon);
        }
        glEnd();
    }

    if ( pPosOff )
    {
        glPopMatrix();
    }
}

#endif


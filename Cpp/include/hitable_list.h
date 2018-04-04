#ifndef HITABLELISTH
#define HITABLELISTH

#include "hitable.h"

class hitable_list: public hitable
{
public:
    hitable_list() : hitable() { list = NULL; list_size = 0; }
    hitable_list( hitable** l, int n ) : hitable() { list = l; list_size = n; }
    virtual bool hit( const ray& r, float t_min, float t_max, hit_record& rec ) const;

    hitable** list;
    int list_size;
};

bool hitable_list::hit( const ray& r, float t_min, float t_max, hit_record& rec ) const
{
    hit_record rec_temp;
    bool hit_something = false;
    float closest = t_max;

    for ( int i = 0; i < list_size; i++ )
    {
        if ( list[i]->hit( r, t_min, closest, rec_temp ) )
        {
            hit_something = true;
            closest = rec_temp.t;
            rec = rec_temp;
        }
    }

    return hit_something;
}

#endif

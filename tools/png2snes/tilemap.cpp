#include "tilemap.h"

TileMap::TileMap()
{
   tileCount=0;
   mapSize=0;
   tileOffset=0;
   hasEmptyTile=false;
   mode = SNES_MAP_MODE;
}

bool TileMap::addTile( Tile* t, int palNum, bool optimizeFlag )
{
    int index= findTile( t );
    
    if ( index == -1 || !optimizeFlag )
    {
        tiles.push_back( t );
        map.push_back( new MapAttribute( mode, (tileOffset + (tileCount++)), palNum) );
    }
    else {
        if ( hasEmptyTile )
            map.push_back( new MapAttribute( mode, index+1, palNum ) );
        else
            map.push_back( new MapAttribute( mode, index, palNum ) );
    }

    mapSize++;
}

int TileMap::findTile( Tile* t )
{
    int i=0;

    while ( i < tiles.size() )
    {
        if ( *tiles[i] == *t )
            return i;
        i++;
    }

    return -1;
}


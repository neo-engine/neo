//#include <nds.h>
//#include <vector>
//#include <fstream>
//#include <sstream>
//#include <n3d/n3dtypes.h>
//
//#include "Map.h"
//#include "mbox.h"
////#include "submesh0_n3dmesh_bin.h"
////#include "test_house_n3dmesh_bin.h"
////
////
////extern Map* acMap;
////const u8* MapData[1][3] = {{submesh0_n3dmesh_bin, test_house_n3dmesh_bin}};
////Map Maps[] = {
////    Map(0,"TEST",16,16,16)
////};
//
//N3DDEVICE g_device;
//
//extern int achours, acseconds, acminutes, acday, acmonth, acyear;
//
//namespace map{
//#define SQ(a) ((a)*(a))   
//    struct vec2 {
//        float x,y;
//        vec2() : x(0),y(0) {}
//        vec2(float x,float y) : x(x),y(y) {}
//        vec2(const vec3& o) : x(o.x),y(o.y) {}
//        
//        vec2 operator -(const vec2& o) const { return vec2(this->x - o.x,this->y - o.y); }
//        vec2 operator +(const vec2& o) const { return vec2(this->x + o.x,this->y + o.y); }
//    };
//    struct vec3 {
//        float x,y,z;
//        vec3() : x(0),y(0),z(0) {}
//        vec3(float x,float y,float z) : x(x),y(y),z(z) {}
//        vec3(const vec4& o) : x(o.x),y(o.y),z(o.z) {}
//        
//        vec3 operator -(const vec3& o) const { return vec3(this->x - o.x,this->y - o.y, this->z - o.z); }
//        vec3 operator +(const vec3& o) const { return vec3(this->x + o.x,this->y + o.y, this->z + o.z); }
//        vec3 operator ^(const vec3& o) const { return vec3(this->y*o.z - this->z*o.y,this->z*o.x - this->x*o.z,this->x*o.y - this->y*o.x); } //Cross Prod
//
//    };
//    vec3 normalize(const vec3& o){ 
//        float l = sqrt(SQ(o.x)+SQ(o.y)+SQ(o.z));
//        return vec3(o.x/l,o.y/l,o.z/l);
//    }
//    struct vec4 {
//        float x,y,z,w;
//        vec4() : x(0),y(0),z(0),w(0) {}
//        vec4(float x,float y,float z,float w) : x(x),y(y),z(z),w(w) {}
//        
//        vec4 operator -(const vec4& o) const { return vec4(this->x - o.x,this->y - o.y, this->z - o.z, this->w - o.w); }
//        vec4 operator +(const vec4& o) const { return vec4(this->x + o.x,this->y + o.y, this->z + o.z, this->w + o.w); }
//    };
//    struct Vertex
//    {
//        vec3 position;
//        vec2 texcoord;
//        vec3 normal;
//    };
//
//    struct VertRef
//    {
//        VertRef( int v, int vt, int vn )
//            : v(v), vt(vt), vn(vn)
//        { }
//        int v, vt, vn;
//    };
//
//    std::vector< Vertex > LoadOBJ( std::istream& in )
//    {
//        std::vector< Vertex > verts;
//
//        std::vector< vec4 > positions( 1, vec4( 0, 0, 0, 0 ) );
//        std::vector< vec3 > texcoords( 1, vec3( 0, 0, 0 ) );
//        std::vector< vec3 > normals( 1, vec3( 0, 0, 0 ) );
//        std::string lineStr;
//        while( std::getline( in, lineStr ) )
//        {
//            std::istringstream lineSS( lineStr );
//            std::string lineType;
//            lineSS >> lineType;
//
//            // vertex
//            if( lineType == "v" )
//            {
//                float x = 0, y = 0, z = 0, w = 1;
//                lineSS >> x >> y >> z >> w;
//                positions.push_back( vec4( x, y, z, w ) );
//            }
//
//            // texture
//            if( lineType == "vt" )
//            {
//                float u = 0, v = 0, w = 0;
//                lineSS >> u >> v >> w;
//                texcoords.push_back( vec3( u, v, w ) );
//            }
//
//            // normal
//            if( lineType == "vn" )
//            {
//                float i = 0, j = 0, k = 0;
//                lineSS >> i >> j >> k;
//                normals.push_back( normalize( vec3( i, j, k ) ) );
//            }
//
//            // polygon
//            if( lineType == "f" )
//            {
//                std::vector< VertRef > refs;
//                std::string refStr;
//                while( lineSS >> refStr )
//                {
//                    std::istringstream ref( refStr );
//                    std::string vStr, vtStr, vnStr;
//                    std::getline( ref, vStr, '/' );
//                    std::getline( ref, vtStr, '/' );
//                    std::getline( ref, vnStr, '/' );
//                    int v = atoi( vStr.c_str() );
//                    int vt = atoi( vtStr.c_str() );
//                    int vn = atoi( vnStr.c_str() );
//                    v  = (  v >= 0 ?  v : positions.size() +  v );
//                    vt = ( vt >= 0 ? vt : texcoords.size() + vt );
//                    vn = ( vn >= 0 ? vn : normals.size()   + vn );
//                    refs.push_back( VertRef( v, vt, vn ) );
//                }
//
//                if( refs.size() < 3 )
//                {
//                    // error, skip
//                    continue;
//                }
//
//                // triangulate, assuming n>3-gons are convex and coplanar
//                VertRef* p[3] = { &refs[0], NULL, NULL };
//                for( size_t i = 1; i+1 < refs.size(); ++i )
//                {
//                    p[1] = &refs[i+0];
//                    p[2] = &refs[i+1];
//
//                    vec3 U( positions[ p[1]->v ] - positions[ p[0]->v ] );
//                    vec3 V( positions[ p[2]->v ] - positions[ p[0]->v ] );
//                    vec3 faceNormal = normalize( U ^ V );
//
//                    for( size_t j = 0; j < 3; ++j )
//                    {
//                        Vertex vert;
//                        vert.position = vec3( positions[ p[j]->v ] );
//                        vert.texcoord = vec2( texcoords[ p[j]->vt ] );
//                        vert.normal   = ( p[j]->vn != 0 ? normals[ p[j]->vn ] : faceNormal );
//                        verts.push_back( vert );
//                    }
//                }
//            }
//        }
//
//        return verts;
//    }
//
//////lightColors
////N3DFLOAT dayTimes[24][3] = {
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,0,0.5},{1,1,1},{1,1,1},
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,1,1},{1,1,1},{1,1,1}};
//////lightPositions
////N3DFLOAT dayPos[24][3] = {
////    {1,1,1},{1,1,1},{1,1,1},
////    {1,0.75,1},{1,0.5,1},{1,0.25,1},
////    {0.75,0,1},{0.5,0,1},{0.25,-0.25,1},
////    {0,-0.25,1},{0,-0.5,0.75},{0,-0.5,0.5},
////    {0,-0.75,0.25},{0,-0.75,0},{0.25,-1,0},
////    {0.5,-0.75,0},{0.75,-0.5,0},{1,-0.25,0.25},
////    {1,0,0.5},{1,0.25,0.75},{1,0.5,1},
////    {1,0.75,1},{1,1,1},{1,1,1}};
////
////void Map::setPos(Point P)
////{
////    if(((P.x/10.0) < (-this->SX/2)) ||( (P.x/10.0) >= (this->SX/2) )||
////        ((P.y/10.0) < 0) || ((P.y/10.0) >= this->SY) ||
////        ((P.z/10.0) < 0) || ((P.z/10.0) >= this->SZ))
////        return;
////    char M = moveData[((P.x/10)+(this->SX/2)) * this->SY + (P.y/10)];
////    if(( M == 0 || M == (2 + (P.z/10) * 8)) && movetype == WALK)
////        this->acPos = P;
////    else if(( M == 0 || M == (3 + (P.z/10) * 8)) && movetype == SURF)
////        this->acPos = P;
////    else if(( M == 0 || M == (6 + (P.z/10) * 8)) && movetype == BIKE)
////        this->acPos = P;
////    if(( M == 0 || M == (7 + (P.z/10) * 8)) && (movetype == WALK || movetype == BIKE))
////        this->acPos = P;
////    else if(M == 1 || M == (4 + (P.z/10) * 8)) // Hero ist gg Wand gelaufen
////        return;
////    else if(M == (5 +(P.z/10) * 8)) 
////    {
////        mbox(""+((P.x/10)) * this->SY + (P.y/10),false);
////        Maps[warpData[((P.x/10)/*+(this->SX/2)*/) * this->SY + (P.y/10)].first].load(warpData[((P.x/10)/*+(this->SX/2)*/) * this->SY +(P.y/10)].second);
////        return;
////    }
////    render();
////}
////void Map::setRot(N3DFLOAT R)
////{
////    rotation = R;
////    render();
////    rotation = N3DFLOAT(0);
////}
////
////void Map::load(Point P)
////{
////    acMap = this;
////    this->acPos = P;
////    render();
////}
////
////void drawHouse(N3DFLOAT x,N3DFLOAT y,N3DFLOAT z,N3DFLOAT rot)
////{
////    // Transform and draw geometry.	
////    N3DMATRIX transMatrix;
////    N3DMATRIX worldMatrix;
////    N3DMATRIX rotationMatrix;
////
////    N3DMatrixRotationY(rotationMatrix, DegreeToRadian(rot));
////    N3DMatrixTranslation(transMatrix, x, y -0.075, z);
////    N3DMatrixMultiply(worldMatrix, rotationMatrix, transMatrix);
////
////    g_device.SetTransform(N3DTS_WORLD, worldMatrix);
////    
////    g_device.DrawPrimitive(*((N3DCMDSTREAM*)(MapData[acMap->index][0])));
////
////    transMatrix = N3DMATRIX();
////    N3DMatrixRotationY(rotationMatrix, DegreeToRadian(rot));
////    N3DMatrixTranslation(transMatrix, x, y +0.3, z);
////    N3DMatrixMultiply(worldMatrix, rotationMatrix, transMatrix);
////
////    g_device.SetTransform(N3DTS_WORLD, worldMatrix);
////    g_device.DrawPrimitive(*((N3DCMDSTREAM*)(MapData[acMap->index][1])));
////}
////
////void Map::render() 
////{
////    // Set the clear color and new depth value, then start the new scene.
////    g_device.Clear(N3DCLEAR_TARGET | N3DCLEAR_DEPTH, N3DCOLOR(0, 0, 1), N3DFLOAT(2.0f));
////    g_device.BeginScene();
////
////    // Setup a light source. Lights in N3D are always directional
////    // lights with an infinite distance.
////    N3DLIGHT light;
////
////    // Set the light color to white
////    light.color.r= N3DFLOAT(dayTimes[achours%24][0]);
////    light.color.g= N3DFLOAT(dayTimes[achours%24][1]);
////    light.color.b= N3DFLOAT(dayTimes[achours%24][2]);
////
////    // Point into the scene
////    light.direction.x = N3DFLOAT(dayPos[achours%24][0]);
////    light.direction.y = N3DFLOAT(dayPos[achours%24][1]);
////    light.direction.z = N3DFLOAT(dayPos[achours%24][2]);
////
////    g_device.SetLight(0, light);
////    g_device.LightEnable(0, true);
////
////
////    // Setup a material
////    N3DMATERIAL material;
////
////    material.ambient.r = N3DFLOAT(0.2f);
////    material.ambient.g = N3DFLOAT(0.2f);
////    material.ambient.b = N3DFLOAT(0.2f);
////
////    material.diffuse.r = N3DFLOAT(0.4f);
////    material.diffuse.g = N3DFLOAT(0.4f);
////    material.diffuse.b = N3DFLOAT(0.6f);
////
////    material.specular.r = N3DFLOAT(0.7f);
////    material.specular.g = N3DFLOAT(0.7f);
////    material.specular.b = N3DFLOAT(1.0f);
////
////    material.emissive.r = N3DFLOAT(0.0f);
////    material.emissive.g = N3DFLOAT(0.0f);
////    material.emissive.b = N3DFLOAT(0.0f);
////
////    // completely opaque
////    material.alpha = N3DFLOAT(1);
////
////    g_device.SetMaterial(material);
////    
////    N3DMATRIX transMatrix;
////    N3DMATRIX worldMatrix;
////    N3DMATRIX rotationMatrix;
////
////    N3DMatrixTranslation(transMatrix, 0, N3DFLOAT(acPos.z / 25.0)+1, 0);
////    N3DMatrixMultiply(worldMatrix, rotationMatrix, transMatrix);
////
////    g_device.SetTransform(N3DTS_WORLD, worldMatrix);
////    drawHouse(N3DFLOAT(acPos.x / 25.0)-1.5, N3DFLOAT(acPos.z / 25.0), N3DFLOAT(acPos.y / 25.0)-3,90);
////    drawHouse(N3DFLOAT(acPos.x / 25.0)+1.5, N3DFLOAT(acPos.z / 25.0), N3DFLOAT(acPos.y / 25.0)-3,0);
////    drawHouse(N3DFLOAT(acPos.x / 25.0)-1.5, N3DFLOAT(acPos.z / 25.0), N3DFLOAT(acPos.y / 25.0)+0,0);
////    drawHouse(N3DFLOAT(acPos.x / 25.0)+1.5, N3DFLOAT(acPos.z / 25.0), N3DFLOAT(acPos.y / 25.0)+0,90);
////
////    // End the scene and present it.
////    g_device.EndScene();
////    g_device.Present();
////}
//}
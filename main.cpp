#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#pragma warning(disable:4996)

#include "StanHull.h"
#include "wavefront.h"

using namespace physx;
using namespace stanhull;

#define USE_POLYGONS 1

int main(int argc,const char **argv)
{
	if ( argc != 2 )
	{
		printf("Usage: StanHull <wavefront.obj>\r\n");
	}
	else
	{
		printf("Loading wavefront obj file '%s'\r\n", argv[1] );
		WavefrontObj obj;
		unsigned int tcount = obj.loadObj(argv[1],false);
		if ( tcount )
		{
			stanhull::HullResult result;
			printf("Creating convex hull.\r\n");
			HullLibrary hl;
#if USE_POLYGONS
			HullDesc desc(QF_DEFAULT,obj.mVertexCount,obj.mVertices,sizeof(PxF32)*3);
#else
			HullDesc desc(QF_TRIANGLES,obj.mVertexCount,obj.mVertices,sizeof(PxF32)*3);
#endif
			HullError err = hl.CreateConvexHull(desc,result);
			if ( err == QE_OK )
			{
				printf("Saving convex hull as 'StanHull.obj'\r\n");
				FILE *fph = fopen("StanHull.obj","wb");
				if ( fph )
				{
					for (PxU32 i=0; i<result.mNumOutputVertices; i++)
					{
						const float *p = &result.mOutputVertices[i*3];
						fprintf(fph,"v %0.9f %0.9f %0.9f\r\n", p[0], p[1], p[2] );
					}
					if ( result.mFaces )
					{
						const PxU32 *indices = result.mIndices;
						for (PxU32 i=0; i<result.mNumFaces; i++)
						{
							PxU32 pcount = result.mFaces[i];
							fprintf(fph,"f ");
							for (PxU32 j=0; j<pcount; j++)
							{
								fprintf(fph,"%d ", indices[0]+1 );
								indices++;
							}
							fprintf(fph,"\r\n");
						}
					}
					else
					{
						for (PxU32 i=0; i<result.mNumFaces; i++)
						{
							PxU32 i1 = result.mIndices[i*3+0];
							PxU32 i2 = result.mIndices[i*3+1];
							PxU32 i3 = result.mIndices[i*3+2];
							fprintf(fph,"f %d %d %d\r\n", i1+1, i2+1, i3+1 );
						}
					}
					fclose(fph);
				}
				hl.ReleaseResult(result);
			}
			else
			{
				printf("failed to create convex hull.\r\n");
			}
		}
		else
		{
			printf("Failed to find any triangles in the input mesh.\r\n");
		}
	}
}

#ifndef OBJFILE_H
#define OBJFILE_H

#ifdef __cplusplus
extern "C"
#endif /* __cplusplus */
bool detectObjFileContent (
	char const * const aFilename ,
	char aoGroupNames [ ] [ 256 ] ,
	unsigned int * const aiNumOfGroupNames ,
	char aoMaterialLibraries [ ] [ 256 ] ,
	unsigned int * const aiNumOfMaterialLibraries ,
	char aoMaterialNames [ ] [ 256 ] ,
	unsigned int * const aiNumOfMaterialNames )
	;


#ifdef __cplusplus
extern "C"
#endif /* __cplusplus */
bool readObjFileGroup (
	char const * const aFilename ,
	char const * const aGroupName ,
	char aoMaterialNames [ ] [ 256 ] ,
	unsigned int * const aiNumOfMaterialNames ,
	float aoVertices [ ] [ 3 ] ,
	unsigned int * const aiNumOfVertices ,
	int aoTriangles [ ] [ 3 ] ,
	unsigned int * const aiNumOfTriangles ,
	float aoTextureVertices [ ] [ 2 ] ,
	unsigned int * const aiNumOfTextureVertices ,
	int aoTextureTriangles [ ] [ 3 ] ,
	unsigned int * const aiNumOfTextureTriangles ,
	int aoTriangleMaterialIndices [ ] ,
	unsigned int * const aiNumOfTriangleMaterialIndices )
	;

#endif /* OBJFILE_H */

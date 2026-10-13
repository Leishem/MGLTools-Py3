#include <assert.h>
#include <fstream>

#include "objfile.h"

extern "C" bool detectObjFileContent (
	char const * const aFilename ,
	char aoGroupNames [ ] [ 256 ] ,
	unsigned int * const aiNumOfGroupNames ,
	char aoMaterialLibraries [ ] [ 256 ] ,
	unsigned int * const aiNumOfMaterialLibraries ,
	char aoMaterialNames [ ] [ 256 ] ,
	unsigned int * const aiNumOfMaterialNames )
{
  char a ;
  unsigned int i ;

	char lTmpName [ 1024 ] ;

  assert ( aFilename ) ;
  assert ( * aFilename != 0 ) ;
  assert ( aiNumOfGroupNames ) ;
  assert ( aiNumOfMaterialLibraries ) ;
  assert ( aiNumOfMaterialNames ) ;

  unsigned int lNumOfGroupNames = 1 ; // 1 as a placeholder for the default
  unsigned int lNumOfMaterialLibraries = 0 ;
  unsigned int lNumOfMaterialNames = 1 ; // 1 as a placeholder for the default material
  strcpy ( aoGroupNames [ 0 ] , "default" ) ;
	strcpy ( aoMaterialNames [ 0 ] , "default" ) ;

  std :: ifstream file_in ( aFilename ) ;
  if ( file_in . eof ( ) )
  {
  	file_in . close ( ) ;
    return false ;
  }

  file_in.get(a);

  while ( ! file_in.eof ( ) )
  {
    if ( a == 0x0D )
    {
      file_in . get ( a ) ;
    }

    while ( ! file_in.eof ( ) && a == 0x0A )
    {
      file_in . get ( a ) ;
    }
	
	  switch ( a )
	  {
      case 'g' :
        if ( * aiNumOfGroupNames <= lNumOfGroupNames )
        {
        	file_in . close ( ) ;
        	return false ;
        }
        file_in >> aoGroupNames [ lNumOfGroupNames ] ;
        
        // we add only groupnames that are not already in the list
        for ( i = 0 ; i < lNumOfGroupNames ; i ++ )
        {
        	if ( 0 == strcmp ( aoGroupNames [ i ] , aoGroupNames [ lNumOfGroupNames ] ) )
        	{
        		break ;
        	}
        }
        if ( i == lNumOfGroupNames ) 
        {    
	      	lNumOfGroupNames ++ ;
        }
        break ;
	    case 'm' :
        file_in . putback ( a ) ;
        file_in >> lTmpName ;
        if ( 0 == strcmp( "mtllib" , lTmpName ) )
        {
        	assert ( aoMaterialLibraries ) ;

					file_in . get ( a ) ;
					while ( ! file_in.eof ( ) && ( a != 0x0D && a != 0x0A ) )
        	{
 				    file_in . putback ( a ) ;
          	file_in >> aoMaterialLibraries [ lNumOfMaterialLibraries ] ;
				    for ( i = 0 ; i < lNumOfMaterialLibraries ; i ++ )
				    {
				    	if ( 0 == strcmp ( aoMaterialLibraries [ i ] , aoMaterialLibraries [ lNumOfMaterialLibraries ] ) )
				    	{
				    		break ;
				    	}
				    }
				    if ( i == lNumOfMaterialLibraries ) 
			    	{
          		lNumOfMaterialLibraries ++ ;
			    	}
          	do 
				    {
				      file_in . get ( a ) ;
				    }
				    while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
        	}
        	file_in . putback ( a ) ;
        }
        break ;
	    case 'u' :
        file_in . putback ( a ) ;
        file_in >> lTmpName ;
        if ( 0 == strcmp( "usemtl" , lTmpName ) )
        {
		      if ( aoMaterialNames )
		      {
          	file_in >> aoMaterialNames [ lNumOfMaterialNames ] ;
				    for ( i = 0 ; i < lNumOfMaterialNames ; i ++ )
				    {
				    	if ( 0 == strcmp ( aoMaterialNames [ i ] , aoMaterialNames [ lNumOfMaterialNames ] ) )
				    	{
				    		break ;
				    	}
				    }
				    if ( i == lNumOfMaterialNames ) 
			    	{
          		lNumOfMaterialNames ++ ;
			    	}
		      }
		      else
		      {
          	lNumOfMaterialNames ++ ;
		      }
	      }
	      break ;
      default : ;
	  }
    do
    {	
      file_in . get ( a ) ;
    }
    while ( ! file_in.eof ( ) && 
		a != 0x0D &&
		a != 0x0A ) ;
  }
  file_in . close ( ) ;

	* aiNumOfGroupNames = lNumOfGroupNames ;
	* aiNumOfMaterialLibraries = lNumOfMaterialLibraries ;
  * aiNumOfMaterialNames = lNumOfMaterialNames ;

  return true ;
}


extern "C" bool readObjFileGroup (
	char const * const aFilename ,
	char const * const aGroupName ,
	char aMaterialNames [ ] [ 256 ] ,
	unsigned int * const aNumOfMaterialNames ,
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
{
  char a ;
  unsigned int i ;

  assert ( aFilename ) ;
  assert ( * aFilename != 0 ) ;
  assert ( aGroupName ) ;
  assert ( * aGroupName != 0 ) ;
  assert ( aNumOfMaterialNames ) ;

  assert ( aiNumOfVertices ) ;
  assert ( aiNumOfTriangles ) ;
  assert ( aiNumOfTextureVertices ) ;
  assert ( aiNumOfTextureTriangles ) ;
  assert ( aiNumOfTriangleMaterialIndices ) ;

  unsigned int lTmpU ;
	char lTmpName [ 1024 ] ;

  char * const & lGroupName = lTmpName ;

  unsigned int lNumOfVertices = 0 ;
  unsigned int lNumOfTriangles = 0 ;
  unsigned int lNumOfTextureVertices = 0 ;
  unsigned int lNumOfTextureTriangles = 0 ;
  unsigned int lNumOfTriangleMaterialIndices = 0 ;
  unsigned int lCurrentMaterialIndex = 0 ;
  
	bool lAskedGroupName ;
	if  (   ( 0 == aGroupName [ 0 ] ) 
	     || ( 0 == strcmp ( aGroupName , "default" ) ) )
	{ 
		lAskedGroupName = true ;
	}
	else
	{ 
		lAskedGroupName = false ;
	}

  std :: ifstream file_in ( aFilename ) ;
  if ( file_in . eof ( ) )
  {
  	file_in . close ( ) ;
    return false ;
  }

  file_in.get(a);

  while ( ! file_in.eof ( ) )
  {
    if ( a == 0x0D )
    {
      file_in . get ( a ) ;
    }

    while ( ! file_in.eof ( ) && a == 0x0A )
    {
      file_in . get ( a ) ;
    }
	
	  switch ( a )
	  {
	    case 'g' : 
        file_in >> lGroupName ;
        if ( 0 == strcmp( aGroupName , lGroupName ) )
        {
          lAskedGroupName = true ;
        }
        else
        {
          lAskedGroupName = false ;
        }
        break ;
	    case 'u' :
        file_in . putback ( a ) ;
        file_in >> lTmpName ;
        if ( 0 == strcmp( "usemtl" , lTmpName ) )
        {
					if ( aMaterialNames ) 
					{
						file_in >> lTmpName ;
				    for ( i = 0 ; i < * aNumOfMaterialNames ; i ++ )
				    {
				    	if ( 0 == strcmp ( aMaterialNames [ i ] , lTmpName ) )
				    	{
				    		lCurrentMaterialIndex = i ;
				    		break ;
				    	}
				    }
				    if ( i == * aNumOfMaterialNames ) 
			    	{
			    		assert ( false ) ;
			    	}
					}
	      }
	      break ;
      case 'v' : 
        file_in . get ( a ) ;
        switch ( a )
        {
          case ' ' :
          	if ( aoVertices )
            {
		          if ( * aiNumOfVertices <= lNumOfVertices )
		          {
		            file_in . close ( ) ;
		            return false ;
		          }
		          file_in >> aoVertices [ lNumOfVertices ] [ 0 ] ;
		          file_in >> aoVertices [ lNumOfVertices ] [ 1 ] ;
		          file_in >> aoVertices [ lNumOfVertices ] [ 2 ] ;
            }
            lNumOfVertices ++ ;
            break ; 
          case 't' :
            if ( aoTextureVertices )
            {
		          if ( * aiNumOfTextureVertices <= lNumOfTextureVertices )
		          {
		            file_in . close ( ) ;
		            return false ;
		          }
		          file_in >> aoTextureVertices [ lNumOfTextureVertices ] [ 0 ] ;
		          file_in >> aoTextureVertices [ lNumOfTextureVertices ] [ 1 ] ;
	            aoTextureVertices [ lNumOfTextureVertices ] [ 1 ] =
	                              1 - aoTextureVertices [ lNumOfTextureVertices ] [ 1 ] ;
            }
            lNumOfTextureVertices ++ ;
            break ; 
          default : ;
        }
        break ;
      case 'f' :
        if ( lAskedGroupName == false )
        {
        	break ;
        }
        file_in . get ( a ) ;
        if ( a == 'o' )
        {
          file_in . get ( a ) ;
        }
        if ( a == ' ' )
        {
          if (aoTriangles)
          {
            if ( * aiNumOfTriangles <= lNumOfTriangles )
            {
          	  file_in . close ( ) ;
          	  return false ;
            }
            file_in >> aoTriangles [ lNumOfTriangles ] [ 0 ] ;
          }
          else
          {
            file_in >> lTmpU ;
          }
          do 
          {
            file_in . get ( a ) ;
          }
          while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
          if ( a == '/' )
          {
            do 
            {
              file_in . get ( a ) ;
            }
            while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
            if ( a == '/' )
            {
              file_in >> lTmpU ;
            }
            else 
            {
              file_in . putback ( a ) ;
              if (aoTextureTriangles)
              {
	            if ( * aiNumOfTextureTriangles <= lNumOfTextureTriangles )
	            {
	              file_in . close ( ) ;
	              return false ;
	            }
                file_in >> aoTextureTriangles [ lNumOfTextureTriangles ] [ 0 ] ;
              }
              else
              {
                file_in >> lTmpU ;
              }
              do
              {
                file_in . get ( a ) ;
              } 
              while ( ! ( 
							  file_in . eof ( ) || 
							  a == ' ' || 
							  a == 0x0D || 
							  a == 0x0A ) ) ; 
            }
          }
          else
          {
          	file_in . putback ( a ) ;
          }

          if (aoTriangles)
          {
            file_in >> aoTriangles [ lNumOfTriangles ] [ 1 ] ;
          }
          else
          {
            file_in >> lTmpU ;
          }
          do
          {
            file_in . get ( a ) ;
          }
          while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
          if ( a == '/' )
          {
            do
            {
              file_in . get ( a ) ;
            } 
            while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
            if ( a == '/' ) 
            {
              file_in >> lTmpU ;
            }
            else 
            {
              file_in . putback ( a ) ;
              if (aoTextureTriangles)
              {
                file_in >> aoTextureTriangles [ lNumOfTextureTriangles ] [ 1 ] ;
              }
              else
              {
                file_in >> lTmpU ;
              }
              do
              {
                file_in . get ( a ) ;
              } 
              while ( ! (
							  file_in . eof ( ) || 
							  a == ' ' || 
							  a == 0x0D || 
							  a == 0x0A ) ) ; 
            }
          }
          else file_in . putback ( a ) ;

          if ( aoTriangles )
          {
            file_in >> aoTriangles [ lNumOfTriangles ] [ 2 ] ;
            aoTriangles [ lNumOfTriangles ] [ 0 ] -- ;
            aoTriangles [ lNumOfTriangles ] [ 1 ] -- ;
            aoTriangles [ lNumOfTriangles ] [ 2 ] -- ;
            
            if ( aoTriangleMaterialIndices )
            {
            	aoTriangleMaterialIndices [ lNumOfTriangles ] = lCurrentMaterialIndex ;
            }
          }
          else
          {
            file_in >> lTmpU ;
          }
          lNumOfTriangles ++ ;
          do 
          {
            file_in . get ( a ) ;
          } 
          while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
          if ( a == '/' )
          {
            do
            {
              file_in . get ( a ) ;
            } 
            while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
            if ( a == '/' )
            {
              file_in >> lTmpU ;
            }
            else
            {
              file_in . putback ( a ) ;
              if ( aoTextureTriangles )
              {
                file_in >> aoTextureTriangles [ lNumOfTextureTriangles ] [ 2 ] ;
		            aoTextureTriangles [ lNumOfTextureTriangles ] [ 0 ] -- ;
		            aoTextureTriangles [ lNumOfTextureTriangles ] [ 1 ] -- ;
		            aoTextureTriangles [ lNumOfTextureTriangles ] [ 2 ] -- ;
              }
              else
              {
                file_in >> lTmpU ;
              }
              lNumOfTextureTriangles ++ ;
            }
            do 
            {
              file_in . get ( a ) ;
            }
            while ( ! (
										file_in . eof ( ) || 
										a == ' ' ||
										a == 0x0D ||
										a == 0x0A ) ) ;
            file_in.putback ( a ) ;
            do
            {
              file_in.get ( a ) ;
            }
            while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
          }
          file_in . putback ( a ) ;

          if ( ! file_in . eof ( ) && 
						   a != 0x0D && 
						   a != 0x0A)
        	{
            if (aoTriangles)
            {
		          if ( * aiNumOfTriangles <= lNumOfTriangles )
		          {
		            file_in . close ( ) ;
		            return false ;
		          }
		          file_in >> aoTriangles [ lNumOfTriangles ] [ 0 ] ;
		          aoTriangles [ lNumOfTriangles ] [ 0 ] -- ;
		          aoTriangles [ lNumOfTriangles ] [ 1 ] = aoTriangles [ lNumOfTriangles - 1 ] [ 0 ] ;
		          aoTriangles [ lNumOfTriangles ] [ 2 ] = aoTriangles [ lNumOfTriangles - 1 ] [ 2 ] ;

	            if ( aoTriangleMaterialIndices )
	            {
	            	aoTriangleMaterialIndices [ lNumOfTriangles ] = lCurrentMaterialIndex ;
	            }
            }
		        else
		        {
		          file_in >> lTmpU ;
		        }
            lNumOfTriangles ++ ;
            do 
            {
              file_in . get ( a ) ;
            } 
            while ( ! file_in.eof ( ) && a == ( a == ' ' || a == '\t' ) ) ;
            if ( a == '/' )
            {
              do 
              { 
                file_in . get ( a ) ;
              }
              while ( ! file_in.eof ( ) && ( a == ' ' || a == '\t' ) ) ;
              if ( a == '/' )
              {
                file_in >> lTmpU ;
              }
              else
              {
                file_in . putback ( a ) ;
                if (aoTextureTriangles)
                {
		              if ( * aiNumOfTextureTriangles <= lNumOfTextureTriangles )
		              {
		                file_in . close ( ) ;
		                return false ;
		              }
	                file_in >> aoTextureTriangles [ lNumOfTextureTriangles ] [ 0 ] ;
				          aoTextureTriangles [ lNumOfTextureTriangles ] [ 0 ] -- ;
				          aoTextureTriangles [ lNumOfTextureTriangles ] [ 1 ] = aoTextureTriangles [ lNumOfTextureTriangles - 1 ] [ 0 ] ;
				          aoTextureTriangles [ lNumOfTextureTriangles ] [ 2 ] = aoTextureTriangles [ lNumOfTextureTriangles - 1 ] [ 2 ] ;
                }
                else
                {
                  file_in >> lTmpU ;
                }
                lNumOfTextureTriangles ++ ;
              }
            }
            else 
            {
              file_in . putback ( a ) ;
            }
          }
        }
        break ;
      default : ;
    }
    do
    {	
      file_in . get ( a ) ;
    }
    while ( ! file_in.eof ( ) && 
		a != 0x0D &&
		a != 0x0A ) ;
  }
  file_in . close ( ) ;

  * aiNumOfVertices = lNumOfVertices ;
  * aiNumOfTriangles = lNumOfTriangles ;
  * aiNumOfTextureVertices = lNumOfTextureVertices ;
  * aiNumOfTextureTriangles = lNumOfTextureTriangles ;

  return true ;
}

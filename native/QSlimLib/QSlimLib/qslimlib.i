%module qslimlib
%feature ("kwargs"); /* enables python keyword arguments swig1.3.20 */

%{
#include <stdmix.h>
#include <mixio.h>
#include <MxSMF.h>
//#include <MxAsp.h>
#include <MxPropSlim.h>
#include <MxQSlim.h>

int placement_policy = MX_PLACE_OPTIMAL;
double boundary_weight = 1000.0;
int weighting_policy = MX_WEIGHT_AREA;
double compactness_ratio = 0.0;
double meshing_penalty = 1.0;
bool will_join_only = false;
typedef MxDynBlock<MxPairContraction> QSlimLog;

 class QSlimModel : public ContractionCallback
{

public:
  QSlimLog *history;
  MxSMFReader smfReader;
  MxStdModel *model;
  MxEdgeQSlim *slim;
  //MxPropSlim *slim;
  QSlimModel( float verts[1][3], int *len1, 
	      int *faces , int len2[2], bool bindtoface = true,
	      bool correctverts = true,
	      float norms[1][3] = NULL, int *len3 = NULL,
	      float colors[1][3] = NULL, int *len4 = NULL,
	      float texcoords[1][2] = NULL, int *len5 = NULL)
  {
    model = new MxStdModel(8,16);
    history = new QSlimLog(100);
    if (norms)
      {
	if (len3[0] == len2[0])
	  bind_normals(1);
	else
	  if (len3[0] == len1[0])
	    bind_normals(0);
      }
    if (colors)
      bind_colors(bindtoface);
    if (texcoords)
      bind_texcoord(0);
    smfReader.createModel(verts, len1, faces , len2, correctverts,
			  norms, len3, colors, len4, texcoords, len5, model);
    //slim = new  MxPropSlim(model);
    slim = new MxEdgeQSlim(*model);
    slim_init();
    slim->cbptr = this;

  }
  
  ~QSlimModel()
  {
    clean_history();
    delete slim;
    delete model;
  }
    
  void clean_history()
  {
    delete history;
    history = NULL;
  }

  void contraction_callback(const MxPairContraction& conx, float cost)
  {
    history->add(conx);
  }
  
  void slim_init()
  {
    slim->placement_policy = placement_policy;
    slim->boundary_weight = boundary_weight;
    slim->weighting_policy = weighting_policy;
    slim->compactness_ratio = compactness_ratio;
    slim->meshing_penalty = meshing_penalty;
    slim->will_join_only = will_join_only;
    
    slim->initialize();
  }
  void bind_normals (bool face)
  {
    if (face)
      model->normal_binding(model->parse_binding("face"));
    else
      model->normal_binding(model->parse_binding("vertex"));
  }
  void bind_colors (bool face)
  {
    if (face)
      model->color_binding(model->parse_binding("face"));
    else
      model->color_binding(model->parse_binding("vertex"));
  }
  void bind_texcoord (bool face)
  {
    if (face)
      model->texcoord_binding(model->parse_binding("face"));
    else
      model->texcoord_binding(model->parse_binding("vertex"));
  }

  int get_color_binding() {return model->color_binding();}
  int get_texcoord_binding() {return model->texcoord_binding();}
  int get_normal_binding() {return model->normal_binding();}
  int get_vert_count() {return model->vert_count();}
  int get_face_count() {return model->face_count();}
  int num_valid_faces() {return slim->valid_faces;}
  int num_valid_verts() {return slim->valid_verts;}
  void slim_to_target(uint target)
  {
    unsigned int status;
    if(target > model->face_count())  target = model->face_count();
    
    if( target < slim->valid_faces )
      {
	status = slim->decimate(target);
	if (!status)
	  cerr << "WARNING: decimate to " << target << " faces failed" << endl;
      }
    else if( target > slim->valid_faces && history )
      {
	while( slim->valid_faces < target && history->length() > 0 )
	  {
	    MxPairContraction& conx = history->drop();
	    slim->apply_expansion(conx);
	  }
      }
  }


  // This function was copied from tools/qslim/main.xx
  void cleanup_for_output()
    
    {
      // First, mark stray vertices for removal
      //
      for(uint i=0; i<model->vert_count(); i++)
	if( model->vertex_is_valid(i) && model->neighbors(i).length() == 0 )
	  model->vertex_mark_invalid(i);
      
      // Compact vertex array so only valid vertices remain
      model->compact_vertices();
    }
  
  void outmodel(float outverts[1][3], int outfaces[1][3],
		float outnorms[1][3] = NULL, float outcolors[1][3] = NULL,
		float outtexcoords[1][2] = NULL)
  {
    unsigned int v, f, i, j = 0;
    for(v=0; v<model->vert_count(); v++)
      {
	// We must output all vertices since numbering is implicit
	MxVertex &vert = model->vertex(v);
	//cout << model->vertex(v) << endl;
	for (i = 0; i<3; i++)
	  outverts[v][i] = vert(i);
	if( model->normal_binding()== MX_PERVERTEX && outnorms )
	  {
	    MxNormal & norm = model->normal(v);
	    outnorms[v][0] = norm[0];
	    outnorms[v][1] = norm[1];
	    outnorms[v][2] = norm[2];
	  }
	if ( model->color_binding()== MX_PERVERTEX && outcolors )
	  {
	    MxColor &col = model->color(v);
	    outcolors[v][0] = col.R();
	    outcolors[v][1] = col.G();
	    outcolors[v][2] = col.B();
	  }
	if( model->texcoord_binding() == MX_PERVERTEX && outtexcoords )
	  {
	    MxTexCoord& tex = model->texcoord(v);
	    outtexcoords[v][0] = tex[0];
	    outtexcoords[v][1] = tex[1];
	  }
      }
    
    for(f=0; f<model->face_count(); f++)
      {
	if( model->face_is_valid(f) )
	  {
	    //cout << model->face(f) << endl;
	    //MxFace &fs =  model->face(f);
	    for(i = 0; i<3; i++)
	      //outfaces[j][i] = fs[i];
	      outfaces[j][i] = model->face(f)(i);
	    if( model->normal_binding()== MX_PERFACE && outnorms )
	      {
		MxNormal & norm = model->normal(f);
		outnorms[j][0] = norm[0];
		outnorms[j][1] = norm[1];
		outnorms[j][2] = norm[2];
	      }
	    if ( model->color_binding()== MX_PERFACE && outcolors )
	      {
		MxColor &col = model->color(f);
		outcolors[j][0] = col.R();
		outcolors[j][1] = col.B();
		outcolors[j][2] = col.G();
	      }
	    if( model->texcoord_binding() == MX_PERFACE && outtexcoords )
	      {
		MxTexCoord& tex = model->texcoord(f);
		outtexcoords[j][0] = tex[0];
		outtexcoords[j][1] = tex[1];
	      }
	    j++;
	  }
      }
  }


void writeSMF(const char* filename)
  {
    MxSMFWriter writer;
    static ostream *output_stream;
    if( filename )
	    output_stream = new ofstream(filename);
    else
      output_stream = &cout;
    writer.write(*output_stream, *model);
  }
};

// this function is used to correct the numbering of vertices
// when vertices are numbered starting from 0.
void init_slim(MxSMFReader *smf)
{
  MxAspVar *avar = smf->asp_store()->lookup("vertex_correction");
  if( avar )
	{
	  avar->read_from("+1", 0);
	}
}



// This function was copied from tools/qslim/main.xx
void cleanup_for_output(MxStdModel *model)

{
    // First, mark stray vertices for removal
    //
    for(uint i=0; i<model->vert_count(); i++)
	if( model->vertex_is_valid(i) && model->neighbors(i).length() == 0 )
	    model->vertex_mark_invalid(i);

	// Compact vertex array so only valid vertices remain
    model->compact_vertices();
}



%}

%include numarr.i


typedef unsigned int uint;

void init_slim(MxSMFReader *smf);


void cleanup_for_output(MxStdModel *m);


FLOAT_ARRAY2D(verts, [1][3], len1)
FLOAT_ARRAY2D(norms, [1][3], len3)
FLOAT_ARRAY2D(colors, [1][3], len4)
FLOAT_ARRAY2D(texcoords, [1][2], len5)

%apply float ARRAY2D[ANY][ANY] { float outverts[1][3] }
%apply float ARRAY2D[ANY][ANY] { float outnorms[1][3] }
%apply float ARRAY2D[ANY][ANY] { float outcolors[1][3] }
%apply float ARRAY2D[ANY][ANY] { float outtexcoords[1][2] }
%apply int ARRAY2D[ANY][ANY] { int outfaces[1][3] }


%typemap(in) (int *faces, int len2[2])(PyArrayObject *array = NULL, int intdims[2])
%{
  array = contiguous_typed_array($input, PyArray_INT, 2, NULL);
  if (! array) return NULL;
  $1 = (int *)array->data;
  intdims[0] = ((PyArrayObject *)(array))->dimensions[0];
  intdims[1] = ((PyArrayObject *)(array))->dimensions[1];
  $2 = intdims;
%}

%typemap(freearg)(int *faces, int len2[2]) %{
if (array$argnum )
      Py_DECREF((PyObject *)array$argnum);
%}


class MxSMFReader : private MxCmdParser
{
public:
    typedef void (MxSMFReader::*read_cmd)(int argc,
					  char *argv[],
					  MxStdModel& m);
    struct cmd_entry { char *name; read_cmd cmd; };

public:
    MxSMFReader();
    ~MxSMFReader();

    bool (*unparsed_hook)(char *cmd, int argc, char *argv[], MxStdModel& m);

    MxAspStore *asp_store() { return this->MxCmdParser::asp_store(); }

    MxStdModel *read(istream& in, MxStdModel *model=NULL);
    void parse_line(char *, MxStdModel&);

    MxStdModel *createModel(  float verts[1][3], int *len1, 
			      int *faces , int len2[2],
			      bool correctverts = true,
			      float norms[1][3] = NULL, int *len3 = NULL,
			      float colors[1][3] = NULL, int *len4 = NULL,
			      float texcoords[1][2] = NULL, int *len5 = NULL,
			      MxStdModel *model = NULL);

};

%extend MxSMFReader
{
  void bind_normals (MxStdModel *m, bool face = true)
    {
      if (face)
	m->normal_binding(m->parse_binding("face"));
      else
	m->normal_binding(m->parse_binding("vertex"));
    }
  void bind_colors (MxStdModel *m, bool face = true)
    {
     if (face)
	m->color_binding(m->parse_binding("face"));
      else
	m->color_binding(m->parse_binding("vertex"));
    }
  void bind_texcoord (MxStdModel *m, bool face = true)
    {
      if (face)
	m->texcoord_binding(m->parse_binding("face"));
      else
	m->texcoord_binding(m->parse_binding("vertex"));
    }

};

class MxSMFWriter
{
public:
    MxSMFWriter();

    void (*vertex_annotate)(ostream&, const MxStdModel&, MxVertexID);
    void (*face_annotate)(ostream&, const MxStdModel&, MxVertexID);

    void write(ostream& out, MxStdModel& m);
};

%extend MxSMFWriter
{
void writeSMF(const char* filename, MxStdModel *m )
  {
    static ostream *output_stream;
    if( filename )
	    output_stream = new ofstream(filename);
    else
      output_stream = &cout;
    self->write(*output_stream, *m);
  }

};


class ContractionCallback
{
};

 class QSlimModel : public ContractionCallback
{

public:
  QSlimLog *history;
  MxSMFReader smfReader;
  MxStdModel *model;
  MxEdgeQSlim *slim;
  //MxPropSlim *slim;
  QSlimModel( float verts[1][3], int *len1, 
	      int *faces , int len2[2], bool bindtoface = true,
	      bool correctverts = true,
	      float norms[1][3] = NULL, int *len3 = NULL,
	      float colors[1][3] = NULL, int *len4 = NULL,
	      float texcoords[1][2] = NULL, int *len5 = NULL);
  ~QSlimModel();
  void clean_history();
  void bind_normals (bool face);
  void bind_colors (bool face);
  void bind_texcoord (bool face);
  int get_color_binding();
  int get_texcoord_binding();
  int get_normal_binding();
  int get_vert_count();
  int get_face_count();
  int num_valid_faces();
  int num_valid_verts();
  void slim_to_target(uint target);
  void contraction_callback(const MxPairContraction& conx, float cost);
  void cleanup_for_output();

  // The following  function is called from the Python world like in the following
  // example:
  // qslimlib.outmodel(newverts, newfaces, outcolors = newcolors,
  //                    outnorms = newnorms, outtexcoords = newtexcoords)
  // where newverts, newfaces, newcolors, newnorms, newtexcoords - Numeric arrays
  void outmodel(float outverts[1][3], int outfaces[1][3],
		float outnorms[1][3]=NULL, float outcolors[1][3]=NULL,
		float outtexcoords[1][2]=NULL);
  void writeSMF(const char* filename=NULL);
};


/*
This function writes one XML VTK file per PID process of type unstructured grid
(*.vtu) which can be read using ParaView (including newer versions). File stores
scalar and vector fields defined at the center points. Results are recorded in
binary format. If one writes one *.vtu file per PID process, this function may be
combined with output_pvtu() to read in parallel. Tested in (quad- and oct-)trees
using MPI. Also works with solids (when not using MPI).
*/

struct OutputFieldsVTU {
  scalar * list;
  vector * vlist;
  char * subname;
};

// Macro for handling periodic boundaries
#define shortcut_periodic()     \
  if (Period.x)                 \
    foreach()                   \
      if (x + Delta > X0 + L0)  \
        per_mask[] = 0.;        \
  if (Period.y)                 \
    foreach()                   \
      if (y + Delta > Y0 + L0)  \
        per_mask[] = 0.;        \
  if (Period.z)                 \
    foreach()                   \
      if (z + Delta > Z0 + L0)  \
        per_mask[] = 0.;        \

void output_vtu_pid (struct OutputFieldsVTU p)
{
  char name[80];
  sprintf(name, "%s.vtu", p.subname);
  FILE * fp = fopen(name, "w");

#if defined(_OPENMP)
  int num_omp = omp_get_max_threads();
  omp_set_num_threads(1);
#endif

  // Initialize mask for periodic boundaries
  scalar per_mask[];
  foreach()
    per_mask[] = 1.;
  shortcut_periodic();

  // Count points and cells, and assign vertex markers
  vertex scalar marker[];
  long no_points = 0, no_cells = 0;
  foreach_vertex(serial, noauto) {
#if TREE
    marker[] = _k;
#else
# if dimension == 2
    marker[] = (point.i-2)*((1 << point.level) + 1) + (point.j-2);
# else
    marker[] = (point.i-2)*sq((1 << point.level) + 1) + (point.j-2)*((1 << point.level) + 1) + (point.k-2);
# endif
#endif
    no_points++;
  }

  foreach(serial, noauto)
    if (per_mask[])
      no_cells++;

  // Write VTK header
  fputs("<?xml version=\"1.0\"?>\n", fp);
  fputs("<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt32\">\n", fp);
  fputs("\t<UnstructuredGrid>\n", fp);
  fprintf(fp, "\t\t<Piece NumberOfPoints=\"%ld\" NumberOfCells=\"%ld\">\n", no_points, no_cells);
  fputs("\t\t\t<CellData Scalars=\"scalars\">\n", fp);

  // Track offsets for appended data
  unsigned int count = 0;
  for (scalar s in p.list) {
    fprintf(fp, "\t\t\t\t<DataArray type=\"Float64\" Name=\"%s\" format=\"appended\" offset=\"%u\"/>\n", s.name, count);
    count += (no_cells * sizeof(double)) + sizeof(unsigned int);
  }

  for (vector v in p.vlist) {
    fprintf(fp, "\t\t\t\t<DataArray type=\"Float64\" Name=\"%s\" NumberOfComponents=\"3\" format=\"appended\" offset=\"%u\"/>\n", v.x.name, count);
    count += (3 * no_cells * sizeof(double)) + sizeof(unsigned int);
  }
  fputs("\t\t\t</CellData>\n", fp);
  fputs("\t\t\t<Points>\n", fp);
  fprintf(fp, "\t\t\t\t<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (3 * no_points * sizeof(double)) + sizeof(unsigned int);
  fputs("\t\t\t</Points>\n", fp);
  fputs("\t\t\t<Cells>\n", fp);

#if dimension == 2
  char type = 9; // VTK_QUAD
  int noffset = 4;
#else
  char type = 11; // VTK_VOXEL (more compatible than VTK_HEXAHEDRON)
  int noffset = 8;
#endif
  long connectivity[noffset];

  fprintf(fp, "\t\t\t\t<DataArray type=\"Int64\" Name=\"connectivity\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (no_cells * noffset * sizeof(long)) + sizeof(unsigned int);
  fprintf(fp, "\t\t\t\t<DataArray type=\"Int32\" Name=\"offsets\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (no_cells * sizeof(int)) + sizeof(unsigned int);
  fprintf(fp, "\t\t\t\t<DataArray type=\"UInt8\" Name=\"types\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (no_cells * sizeof(char)) + sizeof(unsigned int);

  fputs("\t\t\t</Cells>\n", fp);
  fputs("\t\t</Piece>\n", fp);
  fputs("\t</UnstructuredGrid>\n", fp);
  fputs("\t<AppendedData encoding=\"raw\">\n", fp);
  fputs("_", fp);

  // Write heavy data
  unsigned int block_len = no_cells * sizeof(double);
  for (scalar s in p.list) {
    fwrite(&block_len, sizeof(unsigned int), 1, fp);
    foreach(serial, noauto)
      if (per_mask[])
        fwrite(&val(s), sizeof(double), 1, fp);
  }

  block_len = no_cells * 3 * sizeof(double);
  for (vector v in p.vlist) {
    fwrite(&block_len, sizeof(unsigned int), 1, fp);
    foreach(serial, noauto) {
      if (per_mask[]) {
        fwrite(&val(v.x), sizeof(double), 1, fp);
        fwrite(&val(v.y), sizeof(double), 1, fp);
#if dimension == 2
        double vz = 0;
        fwrite(&vz, sizeof(double), 1, fp);
#else
        fwrite(&val(v.z), sizeof(double), 1, fp);
#endif
      }
    }
  }

  block_len = no_points * 3 * sizeof(double);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  foreach_vertex(serial, noauto) {
    fwrite(&x, sizeof(double), 1, fp);
    fwrite(&y, sizeof(double), 1, fp);
    fwrite(&z, sizeof(double), 1, fp);
  }

  block_len = no_cells * noffset * sizeof(long);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  foreach(serial, noauto) {
    if (per_mask[]) {
      connectivity[0] = (long)marker[];
      connectivity[1] = (long)marker[1];
      connectivity[2] = (long)marker[1,1];
      connectivity[3] = (long)marker[0,1];
#if dimension == 3
      connectivity[4] = (long)marker[0,0,1];
      connectivity[5] = (long)marker[1,0,1];
      connectivity[6] = (long)marker[1,1,1];
      connectivity[7] = (long)marker[0,1,1];
#endif
      fwrite(connectivity, sizeof(long), noffset, fp);
    }
  }

  block_len = no_cells * sizeof(int);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  for (int i = 0; i < no_cells; i++) {
    int offset = (i + 1) * noffset;
    fwrite(&offset, sizeof(int), 1, fp);
  }

  block_len = no_cells * sizeof(char);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  for (int i = 0; i < no_cells; i++)
    fwrite(&type, sizeof(char), 1, fp);

  fputs("\n", fp);
  fputs("\t</AppendedData>\n", fp);
  fputs("</VTKFile>\n", fp);
  fflush(fp);
#if defined(_OPENMP)
  omp_set_num_threads(num_omp);
#endif
  fclose(fp);
}

#if _MPI
void output_pvtu(struct OutputFieldsVTU p) {
  char name[112];
  FILE *fp;
  if (pid() == 0) {
    sprintf(name, "%s.pvtu", p.subname);
    fp = fopen(name, "w");
    fputs("<?xml version=\"1.0\"?>\n", fp);
    fputs("<VTKFile type=\"PUnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt32\">\n", fp);
    fputs("\t<PUnstructuredGrid GhostLevel=\"0\">\n", fp);
    fputs("\t\t<PCellData Scalars=\"scalars\">\n", fp);
    for (scalar s in p.list) {
      fprintf(fp, "\t\t\t<PDataArray type=\"Float64\" Name=\"%s\" format=\"appended\"/>\n", s.name);
    }
    for (vector v in p.vlist) {
      fprintf(fp, "\t\t\t<PDataArray type=\"Float64\" NumberOfComponents=\"3\" Name=\"%s\" format=\"appended\"/>\n", v.x.name);
    }
    fputs("\t\t</PCellData>\n", fp);
    fputs("\t\t<PPoints>\n", fp);
    fputs("\t\t\t<PDataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"appended\"/>\n", fp);
    fputs("\t\t</PPoints>\n", fp);
    for (int i = 0; i < npe(); i++)
      fprintf(fp, "\t\t<Piece Source=\"%s_n%3.3d.vtu\"/>\n", p.subname, i);
    fputs("\t</PUnstructuredGrid>\n", fp);
    fputs("</VTKFile>\n", fp);
    fflush(fp);
    fclose(fp);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  sprintf(name, "%s_n%3.3d", p.subname, pid());
  output_vtu_pid(p);
}
#endif

trace
void output_vtu (struct OutputFieldsVTU p)
{
#if _MPI
  output_pvtu(p);
#else
  output_vtu_pid(p);
#endif
}

struct OutputSlicesVTU {
  scalar * list;
  vector * vlist;
  char * subname;
  coord n;
  double _alpha;
};

#define shortcut_slice(n,_alpha) \
  double alpha = (_alpha - n.x*x - n.y*y - n.z*z)/Delta; \
  if (fabs(alpha) > 0.87) \
    continue;

#if dimension > 2
void output_vtu_plane_pid (struct OutputSlicesVTU p) {
#if defined(_OPENMP)
  int num_omp = omp_get_max_threads();
  omp_set_num_threads(1);
#endif

  char name[80];
  sprintf(name, "%s.vtu", p.subname);
  FILE * fp = fopen(name, "w");

  coord n = p.n;
  double _alpha = p._alpha;
  scalar per_mask[];
  foreach() {
    per_mask[] = 0.;
    shortcut_slice(n,_alpha);
    if (alpha > 0.)
      per_mask[] = 1.;
  }
  shortcut_periodic();

  vertex scalar marker[];
  long no_points = 0, no_cells = 0;
  foreach_vertex(serial, noauto) {
    marker[] = 0.;
    shortcut_slice(n,_alpha);
    marker[] = no_points;
    no_points++;
  }
  foreach(serial, noauto)
    if (per_mask[])
      no_cells++;

  fputs("<?xml version=\"1.0\"?>\n", fp);
  fputs("<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt32\">\n", fp);
  fputs("\t<UnstructuredGrid>\n", fp);
  fprintf(fp, "\t\t<Piece NumberOfPoints=\"%ld\" NumberOfCells=\"%ld\">\n", no_points, no_cells);
  fputs("\t\t\t<CellData Scalars=\"scalars\">\n", fp);

  unsigned int count = 0;
  for (scalar s in p.list) {
    fprintf(fp, "\t\t\t\t<DataArray type=\"Float64\" Name=\"%s\" format=\"appended\" offset=\"%u\"/>\n", s.name, count);
    count += (no_cells * sizeof(double)) + sizeof(unsigned int);
  }
  for (vector v in p.vlist) {
    fprintf(fp, "\t\t\t\t<DataArray type=\"Float64\" Name=\"%s\" NumberOfComponents=\"3\" format=\"appended\" offset=\"%u\"/>\n", v.x.name, count);
    count += (3 * no_cells * sizeof(double)) + sizeof(unsigned int);
  }
  fputs("\t\t\t</CellData>\n", fp);
  fputs("\t\t\t<Points>\n", fp);
  fprintf(fp, "\t\t\t\t<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (3 * no_points * sizeof(double)) + sizeof(unsigned int);
  fputs("\t\t\t</Points>\n", fp);
  fputs("\t\t\t<Cells>\n", fp);

  char type = 9; // VTK_QUAD for slices
  int noffset = 4;
  fprintf(fp, "\t\t\t\t<DataArray type=\"Int64\" Name=\"connectivity\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (no_cells * noffset * sizeof(long)) + sizeof(unsigned int);
  fprintf(fp, "\t\t\t\t<DataArray type=\"Int32\" Name=\"offsets\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (no_cells * sizeof(int)) + sizeof(unsigned int);
  fprintf(fp, "\t\t\t\t<DataArray type=\"UInt8\" Name=\"types\" format=\"appended\" offset=\"%u\"/>\n", count);
  count += (no_cells * sizeof(char)) + sizeof(unsigned int);

  fputs("\t\t\t</Cells>\n", fp);
  fputs("\t\t</Piece>\n", fp);
  fputs("\t</UnstructuredGrid>\n", fp);
  fputs("\t<AppendedData encoding=\"raw\">\n", fp);
  fputs("_", fp);

  unsigned int block_len = no_cells * sizeof(double);
  for (scalar s in p.list) {
    fwrite(&block_len, sizeof(unsigned int), 1, fp);
    foreach(serial, noauto) {
      if (per_mask[]) {
        double sval;
        if (n.x == 1)
          sval = 0.5*(val(s) + val(s,1,0,0));
        else if (n.y == 1)
          sval = 0.5*(val(s) + val(s,0,1,0));
        else
          sval = 0.5*(val(s) + val(s,0,0,1));
        fwrite(&sval, sizeof(double), 1, fp);
      }
    }
  }

  block_len = no_cells * 3 * sizeof(double);
  for (vector v in p.vlist) {
    fwrite(&block_len, sizeof(unsigned int), 1, fp);
    foreach(serial, noauto) {
      if (per_mask[]) {
        double xval, yval, zval;
        if (n.x == 1) {
          xval = 0.5*(val(v.x) + val(v.x,1,0,0));
          yval = 0.5*(val(v.y) + val(v.y,1,0,0));
          zval = 0.5*(val(v.z) + val(v.z,1,0,0));
        } else if (n.y == 1) {
          xval = 0.5*(val(v.x) + val(v.x,0,1,0));
          yval = 0.5*(val(v.y) + val(v.y,0,1,0));
          zval = 0.5*(val(v.z) + val(v.z,0,1,0));
        } else {
          xval = 0.5*(val(v.x) + val(v.x,0,0,1));
          yval = 0.5*(val(v.y) + val(v.y,0,0,1));
          zval = 0.5*(val(v.z) + val(v.z,0,0,1));
        }
        fwrite(&xval, sizeof(double), 1, fp);
        fwrite(&yval, sizeof(double), 1, fp);
        fwrite(&zval, sizeof(double), 1, fp);
      }
    }
  }

  block_len = no_points * 3 * sizeof(double);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  foreach_vertex(serial, noauto) {
    shortcut_slice(n,_alpha);
    fwrite(&x, sizeof(double), 1, fp);
    fwrite(&y, sizeof(double), 1, fp);
    fwrite(&z, sizeof(double), 1, fp);
  }

  block_len = no_cells * noffset * sizeof(long);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  foreach(serial, noauto) {
    long connectivity[noffset];
    if (per_mask[]) {
      if (n.x == 1) {
        connectivity[0] = (long)marker[1,0,0];
        connectivity[1] = (long)marker[1,1,0];
        connectivity[2] = (long)marker[1,1,1];
        connectivity[3] = (long)marker[1,0,1];
      } else if (n.y == 1) {
        connectivity[0] = (long)marker[0,1,0];
        connectivity[1] = (long)marker[1,1,0];
        connectivity[2] = (long)marker[1,1,1];
        connectivity[3] = (long)marker[0,1,1];
      } else {
        connectivity[0] = (long)marker[0,0,1];
        connectivity[1] = (long)marker[1,0,1];
        connectivity[2] = (long)marker[1,1,1];
        connectivity[3] = (long)marker[0,1,1];
      }
      fwrite(connectivity, sizeof(long), noffset, fp);
    }
  }

  block_len = no_cells * sizeof(int);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  for (int i = 0; i < no_cells; i++) {
    int offset = (i + 1) * noffset;
    fwrite(&offset, sizeof(int), 1, fp);
  }

  block_len = no_cells * sizeof(char);
  fwrite(&block_len, sizeof(unsigned int), 1, fp);
  for (int i = 0; i < no_cells; i++)
    fwrite(&type, sizeof(char), 1, fp);

  fputs("\n", fp);
  fputs("\t</AppendedData>\n", fp);
  fputs("</VTKFile>\n", fp);
  fflush(fp);
#if defined(_OPENMP)
  omp_set_num_threads(num_omp);
#endif
  fclose(fp);
}

#if _MPI
void output_pvtu_plane (struct OutputSlicesVTU p) {
  char name[99];
  FILE * fp;
  if (pid() == 0) {
    sprintf(name, "%s.pvtu", p.subname);
    fp = fopen(name, "w");
    fputs("<?xml version=\"1.0\"?>\n", fp);
    fputs("<VTKFile type=\"PUnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt32\">\n", fp);
    fputs("\t<PUnstructuredGrid GhostLevel=\"0\">\n", fp);
    fputs("\t\t<PCellData Scalars=\"scalars\">\n", fp);
    for (scalar s in p.list) {
      fprintf(fp, "\t\t\t<PDataArray type=\"Float64\" Name=\"%s\" format=\"appended\"/>\n", s.name);
    }
    for (vector v in p.vlist) {
      fprintf(fp, "\t\t\t<PDataArray type=\"Float64\" NumberOfComponents=\"3\" Name=\"%s\" format=\"appended\"/>\n", v.x.name);
    }
    fputs("\t\t</PCellData>\n", fp);
    fputs("\t\t<PPoints>\n", fp);
    fputs("\t\t\t<PDataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"appended\"/>\n", fp);
    fputs("\t\t</PPoints>\n", fp);
    for (int i = 0; i < npe(); i++)
      fprintf(fp, "\t\t<Piece Source=\"%s_n%3.3d.vtu\"/>\n", p.subname, i);
    fputs("\t</PUnstructuredGrid>\n", fp);
    fputs("</VTKFile>\n", fp);
    fflush(fp);
    fclose(fp);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  sprintf(name, "%s_n%3.3d", p.subname, pid());
  output_vtu_plane_pid(p);
}
#endif

trace
void output_slice_vtu (struct OutputSlicesVTU p)
{
#if _MPI
  output_pvtu_plane(p);
#else
  output_vtu_plane_pid(p);
#endif
}
#endif
#undef shortcut_slice
#undef shortcut_periodic
# Install Basilisk

[Install basilisk script](1.1_Install_Basilisk.sh)

# Rules
save all dump file in intermediate.

# convet to vtu file
[output_vut](basilisk_files\output_vtu_foreach.h)
# Some tips

## restore and dump function
when use "**p.nodump = false;**", the restore function need become "**restore (file = "dump",list = all)**", the mpirun running will fail but openmp works.
```
event init(t = 0)
{
  if(!restore (file = "dump",list = all)){
    refine(R2Drop(x, y) < 1.05 && (level < MAXlevel));
    fraction(f, 1. - R2Drop(x, y));
    foreach ()
    {
      u.x[] = -1.0 * f[];
      u.y[] = 0.0;
    }
    boundary((scalar *){f, u.x, u.y});
  }
}

event writingFiles(t += tsnap)
{
  if (i==0){
    char comm[80];
    sprintf(comm, "mkdir -p intermediate");
    system(comm);
  }
  p.nodump = false;
  dump(file = "dump");
  sprintf(nameOut, "intermediate/snapshot-%5.4f", t);
  dump(file = nameOut);
}
```

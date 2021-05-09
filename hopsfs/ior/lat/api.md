```cpp
typedef struct ior_aiori
{
    char *name;
    char *name_legacy;
    void *(*create)(char *, IOR_param_t *);
    int (*mknod)(char *);
    void *(*open)(char *, IOR_param_t *);
    IOR_offset_t (*xfer)(int, void *, IOR_size_t *,
                         IOR_offset_t, IOR_param_t *);
    void (*close)(void *, IOR_param_t *);
    void (*delete)(char *, IOR_param_t *);
    char *(*get_version)(void);
    void (*fsync)(void *, IOR_param_t *);
    IOR_offset_t (*get_file_size)(IOR_param_t *, MPI_Comm, char *);
    int (*statfs)(const char *, ior_aiori_statfs_t *, IOR_param_t *param);
    int (*mkdir)(const char *path, mode_t mode, IOR_param_t *param);
    int (*rmdir)(const char *path, IOR_param_t *param);
    int (*access)(const char *path, int mode, IOR_param_t *param);
    int (*stat)(const char *path, struct stat *buf, IOR_param_t *param);
    void (*initialize)(void);                                                    /* called once per program before MPI is started */
    void (*finalize)(void);                                                      /* called once per program after MPI is shutdown */
    option_help *(*get_options)(void **init_backend_options, void *init_values); /* initializes the backend options as well and returns the pointer to the option help structure */
    bool enable_mdtest;
    int (*check_params)(IOR_param_t *); /* check if the provided parameters for the given test and the module options are correct, if they aren't print a message and exit(1) or return 1*/
    void (*sync)(IOR_param_t *);        /* synchronize every pending operation for this storage */
} ior_aiori_t;

///////////////////////////////////////////////

double st = MPI_Wtime();
double ed = MPI_Wtime();
fout_mkdir_latency << ed - st << std::endl;
{
    mkdir;
}

{
    double st = MPI_Wtime();
    double ed = MPI_Wtime();
    fout_statdir_latency << ed - st << std::endl;

    double st = MPI_Wtime();
    double ed = MPI_Wtime();
    fout_stat_latency << ed - st << std::endl;
}
{
    stat;
}

double st = MPI_Wtime();
double ed = MPI_Wtime();
fout_rmdir_latency << ed - st << std::endl;

double st = MPI_Wtime();
double ed = MPI_Wtime();
fout_unlink_latency << ed - st << std::endl;
{
    delete;
}

double st = MPI_Wtime();
double ed = MPI_Wtime();
fout_open_latency << ed - st << std::endl;
{
    open;
}

double st = MPI_Wtime();
double ed = MPI_Wtime();
fout_create_latency << ed - st << std::endl;
{
    mknod; //!!! only to file
    create;
    open(O_CREAT); //!!!! nononon; open(IOR_CREAT) is done all by create()
}

{
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
    ;
}

fout_rmdir_latency << ed - st << std::endl;
fprintf(fout_rmdir_latency, "%f\n", ed - st);

fout_mkdir_latency << ed - st << std::endl;
fprintf(fout_mkdir_latency, "%f\n", ed - st);

fout_statdir_latency << ed - st << std::endl;
fprintf(fout_statdir_latency, "%f\n", ed - st);

fout_unlink_latency << ed - st << std::endl;
fprintf(fout_unlink_latency, "%f\n", ed - st);

fout_open_latency << ed - st << std::endl;
fprintf(fout_open_latency, "%f\n", ed - st);

fout_create_latency << ed - st << std::endl;
fprintf(fout_create_latency, "%f\n", ed - st);

fout_stat_latency << ed - st << std::endl;
fprintf(fout_stat_latency, "%f\n", ed - st);
```
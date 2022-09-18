#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define PRINT 1

int SZ = 8;
int *v;

// Declare variable to hold memory object (used for the vector)
cl_mem bufV;

// Declare variable to hold unique device identifier
cl_device_id device_id;

// Declare variable to hold context
cl_context context;

// Declare variable to hold program 
cl_program program;

// Declare variable to hold kernel
cl_kernel kernel;

// Declare variable to hold command queue
cl_command_queue queue;

// Declare variable to hold command event
cl_event event = NULL;

int err;

// Find devices that can be used  
cl_device_id create_device();

// Create context and attach device/command queue/built kernel code.
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);

// Read program file (vector_ops.cl) to create program in context
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);

/* Creates memory object (to hold the vector to operate on) and then adds command
 to write it to the device */
void setup_kernel_memory();

/* Sets the values of the kernel arguments (size of vector and memory object
 holding the vector to operate on) */
void copy_kernel_args();

// Frees up memory of the host program
void free_memory();

void init(int *&A, int size);
void print(int *A, int size);

int main(int argc, char **argv)
{
    if (argc > 1)
        SZ = atoi(argv[1]);

    init(v, SZ);


    // Configure number of global work-items to SZ (size of vector)
    size_t global[1] = {(size_t)SZ};

    //initial vector
    print(v, SZ);

    setup_openCL_device_context_queue_kernel((char *)"./vector_ops.cl", (char *)"square_magnitude");

    setup_kernel_memory();
    copy_kernel_args();

    /*
    Add command to execute kernel to the command queue
    Arguments:
      cl_command_queue command_queue: The command queue to add to.
  	  cl_kernel kernel: The kernel to execute in this command.
  	  cl_uint work_dim: Number of dimensions used to specify global work items (1-3)
  	  const size_t *global_work_offset: Must be NULL, not supported yet.
  	  const size_t *global_work_size: Array of size "work_dim", each element is a dimension with the number
                                      of items representing the number of global work items.
  	  const size_t *local_work_size: Same as global_work_size but for the number of work items to execute the kernel
                                     in "kernel" arg. Can be NULL for OpenCL to divide global work items automatically.
  	  cl_uint num_events_in_wait_list: Number of events that need to be completed before command can execute.
  	  const cl_event *event_wait_list: Array of events of size "num_events_in_wait_list" that need to be completed before command can execute.
  	  cl_event *event: Returns event object for this command (others command can wait on this...)
    */
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);
    clWaitForEvents(1, &event);

    /*
    Queue command to copy from memory object to host memory.
    Arguments:
      cl_command_queue command_queue: The command queue to add to.
  	  cl_mem buffer: The memory object to read from.
  	  cl_bool blocking_read: Whether read is blocking or not.
  	  size_t offset: Offset in bytes (of the memory object) to read from.
  	  size_t cb: Size in bytes of data being read.
  	  void *ptr: Pointer to buffer in host memory that we will copy data into.
  	  cl_uint num_events_in_wait_list: Number of events that need to be completed before command can execute.
  	  const cl_event *event_wait_list: Array of events of size "num_events_in_wait_list" that need to be completed before command can execute.
  	  cl_event *event: Returns event object for this command (others command can wait on this...)
    */
    clEnqueueReadBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);

    //result vector
    print(v, SZ);

    //frees memory for device, kernel, queue, etc.
    //you will need to modify this to free your own buffers
    free_memory();
}

void init(int *&A, int size)
{
    A = (int *)malloc(sizeof(int) * size);

    for (long i = 0; i < size; i++)
    {
        A[i] = rand() % 100; // any number less than 100
    }
}

void print(int *A, int size)
{
    if (PRINT == 0)
    {
        return;
    }

    if (PRINT == 1 && size > 15)
    {
        for (long i = 0; i < 5; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
        printf(" ..... ");
        for (long i = size - 5; i < size; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
    }
    else
    {
        for (long i = 0; i < size; i++)
        {                        //rows
            printf("%d ", A[i]); // print the cell value
        }
    }
    printf("\n----------------------------\n");
}

void free_memory()
{
    //free the buffers
    clReleaseMemObject(bufV);

    //free opencl objects
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(v);
}


void copy_kernel_args()
{
    /*
    Set value for specific argument of kernel
    Arguments:
      cl_kernel kernel: Kernel object to be affected.
      cl_uint arg_index: Index of the argument to be set (0 is leftmost arg).
      size_t arg_size: Size of the argument value (for memory object this is size of buffer).
      const void *arg_value: Pointer to the value will be set for kernel arg.
    */
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&SZ);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufV);

    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setup_kernel_memory()
{
    /*
    Creates a memory object inside the context
    Arguments:
      cl_context context: Context to create memory object in.
      cl_mem_flags flags: Type of allocation/usage, default is CL_MEM_READ_WRITE which means
        read/written by kernel & more options include CL_MEM_WRITE_ONLY and CL_MEM_READ_ONLY
        which limit write/read only by kernel. We also have CL_MEM_USE_HOST_PTR and others
        which work in conjuction with host_ptr.
      size_t size: Size in bytes of memory to be allocated.
      void *host_ptr: Pointer to data which can be allocated already by application (NULL if not needed)
      cl_int *errcode_ret: Pointer to hold error code (NULL if successful)
    */
    bufV = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);
}

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    /* Creates context, this is an environment to hold the memory, programs, command 
     queues, and kernels which will be executed in it. */
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);

    /* Creates command queue in the context created above for the device created 
     above (identified by "device_id") */
    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    };


    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    };
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    /*
      Creates program for context, loads in source code specified by text strings.
      Arguments:
        cl_context context: Context to create program in.
        cl_unint count: Total number of strings (i.e. size of the array below).
        const char **strings: Array of pointers to character strings (corresponding to the source code).
        const size_t *lengths: Number of characters in each strength.
        cl_int *errcode_ret: Pointer to hold error code (NULL if successful).
    */
    program = clCreateProgramWithSource(ctx, 1,
                                        (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program 

   The fourth parameter accepts options that configure the compilation. 
   These are similar to the flags used by gcc. For example, you can 
   define a macro with the option -DMACRO=VALUE and turn off optimization 
   with -cl-opt-disable.
   */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0)
    {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &log_size);
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id dev;
   int err;

   /* Identify a platform */
   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      printf("GPU not found\n");
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}
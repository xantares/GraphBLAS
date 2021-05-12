
//#includePath += "../rmm/include/rmm"
#include <rmm_wrap.h>

inline auto make_cuda() { return std::make_shared<rmm::mr::cuda_memory_resource>(); }

inline auto make_managed() { return std::make_shared<rmm::mr::managed_memory_resource>();}

inline auto make_and_set_pool(size_t initial_size, size_t maximum_size) 
{ 
	auto resource = rmm::mr::make_owning_wrapper<rmm::mr::pool_memory_resource>
			        ( make_managed(), initial_size, maximum_size );
	rmm::mr::set_current_device_resource( resource.get());
	return resource;
}

typedef rmm::mr::managed_memory_resource managed_mr;
typedef rmm::mr::pool_memory_resource<managed_mr> managed_pool_mr;

struct RMM_Handle 
{
	rmm::mr::device_memory_resource *rmm;
	//std::shared_ptr<rmm::mr::owning_wrapper<managed_pool_mr, managed_mr>> resource; 
	std::shared_ptr<rmm::mr::device_memory_resource> resource; 


	void make_pool_and_set_size(size_t initial_size, size_t maximum_size) 
	{ 
	    printf("in make and set pool, %d %d\n", initial_size, maximum_size);
		this->resource = make_and_set_pool(initial_size, maximum_size);// rmm::mr::make_owning_wrapper<rmm::mr::pool_memory_resource> ( make_managed(), initial_size, maximum_size );
	    printf("done, %ld \n", this->resource);
	}
};

void rmm_create_handle( RMM_Handle *handle)
{ //place a new RMM pool at this handle in memory
  handle = new RMM_Handle(); 
}

void rmm_destroy_handle ( RMM_Handle *handle)
{
  free(handle);
}

void rmm_initialize(RMM_Handle *handle, size_t init_pool_size, size_t max_pool_size)
{
    // Construct a resource that uses a coalescing best-fit pool allocator
	handle->make_pool_and_set_size( init_pool_size, max_pool_size);
	printf("returned from make and set\n");
	//handle->rmm = rmm::mr::get_current_device_resource();
}


void *rmm_allocate( RMM_Handle *handle , size_t size)
{
    //    static_cast<rmm::mr::pool_memory_resource<rmm::mr::managed_memory_resource>*>(mr)
    // Constructs an adaptor that forwards all (de)allocations to `mr` on `stream`.
    // stream 0 by default here
	//rmm::mr::device_memory_resource *mymr{rmm::mr::get_current_device_resource()};
    return handle->resource.get()->allocate( size );
}

void rmm_deallocate( RMM_Handle* handle, void *p, size_t size)
{
    //	rmm::mr::device_memory_resource *mymr{rmm::mr::get_current_device_resource()};
    handle->resource.get()->deallocate( p, size );
}


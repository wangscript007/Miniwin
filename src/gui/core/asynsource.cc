#include <asynsource.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

namespace nglui {

class ThreadPool {
  public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
    size_t size(){return tasks.size();}
  private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers;
    // the task queue
    std::queue< std::function<void()> > tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t threads)
    :   stop(false) {
    for(size_t i = 0; i<threads; ++i)
        workers.emplace_back(
        [this] {
        for(;;) {
            std::function<void()> task;

            {
                std::unique_lock<std::mutex> lock(this->queue_mutex);
                this->condition.wait(lock,
                [this] { return this->stop || !this->tasks.empty(); });
                if(this->stop && this->tasks.empty())
                    return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
            }

            task();
        }
    }
    );
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker: workers)
        worker.join();
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared< std::packaged_task<return_type()> >(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() {
            (*task)();
        });
    }
    condition.notify_one();
    return res;
}


AsyncEventSource::AsyncEventSource(){
    int num=std::thread::hardware_concurrency();
    pool=new ThreadPool(num);
}

AsyncEventSource::~AsyncEventSource() {
    delete pool;
}

bool AsyncEventSource::prepare(int& max_timeout) {
    return pool->size();
}
class TEST{
public:
  static void CHECK(int a,int b){}
};
bool AsyncEventSource::check(){
    pool->enqueue(TEST::CHECK,1,2);
    return pool->size()>0;
}

/*void AsyncEventSource::enqueue(F&& f, Args&&... args){
     pool->enqueue(f,args);
}*/

bool AsyncEventSource::dispatch(EventHandler &func) { return func(*this); }


}

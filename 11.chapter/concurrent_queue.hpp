#include <queue> 
#include <mutex> 
#include <condition_variable> 

template<typename Data>
class concurrent_queue
{
private:
    std::queue<Data> the_queue;
    mutable std::mutex the_mutex;
    std::condition_variable the_condition_variable;
public:
    void push(Data const& data)
    {
        std::unique_lock<std::mutex> lock(the_mutex);
        the_queue.push(data);
        lock.unlock();
        the_condition_variable.notify_one();
    }

    bool empty() const
    {
        std::unique_lock<std::mutex> lock(the_mutex);
        return the_queue.empty();
    }

    size_t size() const
    {
        std::unique_lock<std::mutex> lock(the_mutex);
        return the_queue.size();
    }

    bool try_pop(Data& popped_value)
    {
        std::unique_lock<std::mutex> lock(the_mutex);
        if(the_queue.empty())
        {
            return false;
        }
        
        popped_value=the_queue.front();
        the_queue.pop();
        return true;
    }

    void wait_and_pop(Data& popped_value)
    {
        std::unique_lock<std::mutex> lock(the_mutex);
        while(the_queue.empty())
        {
            the_condition_variable.wait(lock);
        }
        
        popped_value=the_queue.front();
        the_queue.pop();
    }

};

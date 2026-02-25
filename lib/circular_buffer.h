#include <memory>

template<typename T, bool Extendable = false, typename Allocator = std::allocator<T>>
class circular_buffer {
private:

    Allocator alloc_;

    T* data_=nullptr;
    size_t capacity_{};
    size_t size_{};

    size_t write_ind_{};
    size_t read_ind_{};

    class iterator{
        circular_buffer* buffer_;
        T* ptr_;

    };

    class const_iterator{

    }

    void Extend();

public:

    using value_type = T;
    using reference = T&;
    using const_reference=const T&;
    using size_type = size_t;
    


    circular_buffer(size_t capacity);

    circular_buffer(std::initializer_list<T> elements);

    circular_buffer(const circular_buffer& other);

    circular_buffer(circular_buffer&& other);

    size_t size(){ return size_; };

    size_t max_size(){return capacity_;}

    bool empty(){return size_==0};

    void clear();

    template<typename U>
    void push_back(U&& element);

    void pop_back();

    template<typename U>
    void push_front(U&& element);

    void pop_front();

    void insert();

    void erase();

    iterator begin(){return {};}
    iterator end(){return {};}

    const_iterator begin() const {return {};}
    const_iterator end() const {return {};}

    const_iterator cbegin() const {return {};}
    const_iterator cend() const {return {};}

};

template<typename T, bool Extendable, typename Allocator>
void circular_buffer<T,Extendable, Allocator>::Extend(){
    if(capacity_==0){
        data=std::allocator_traits::allocate(5);
        capacity_=5;
    }
    else{
        T* new_data = std::allocator_traits::allocate(capacity_*2);
        auto it = begin();
        for(int i = 0; i < size_; i++){
            new_data[i]=std::move(*it);
            it++;
        }
        std::allocator_traits::deallocate(alloc_, data_);
        data_=new_data;
        write_ind_=size_;
        read_ind_=0;
        capacity_*=2;
    }
}


template<typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(size_t capacity){
    data_=std::allocator_traits<Allocator>::allocate(alloc_, capacity);
    capacity_=capacity;
}


template<typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(std::initializer_list<T> elements){

    data_=std::allocator_traits<Allocator>::allocate(alloc_, elements.size());
    for(auto it = elements.begin(); it != elements.end(); it++){
        std::allocator_traits<Allocator>::construct(alloc_, data_+write_ind_, *it);
        write_ind_++;
    }
    size_=elements.size();
    capacity_=size_;
    write_ind_=size_;
}


template<typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(const circular_buffer& other){
    data_=std::allocator_traits<Allocator>::allocate(alloc_, other.capacity_);
    for(auto it = other.begin(); it != other.end(); it++){
        std::allocator_traits<Allocator>::construct(alloc_, data_+write_ind_, *it);
        write_ind_++;
    }
    size_=other.size_;
    capacity_=other.capacity_;
}


template<typename T, bool Extendable, typename Allocator>
circular_buffer<T, Extendable, Allocator>::circular_buffer(circular_buffer&& other){
    data_=std::allocator_traits<Allocator>::allocate(alloc_, other.capacity_);
    for(auto it = other.begin(); it != other.end(); it++){
        std::allocator_traits<Allocator>::construct(alloc_, data_+write_ind_, std::move(*it));
        write_ind_++;
    }
    size_=other.size_;
    capacity_=other.capacity_;
}




template<typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::clear(){

    for(auto it = begin(); it!=end(); it++){
        std::iterator_traits<Allocator>::destroy(alloc_, it.ptr_);
    }
    write_ind_=0;
    read_ind_=0;
    size_=0;

}


template<typename T, bool Extendable, typename Allocator>
template<typename U>
void circular_buffer<T, Extendable, Allocator>::push_back(U&& element){

    if(size_==capacity_){
        Extend();
    }

    std::allocator_traits<Allocator>::construct(alloc_, data_+write_ind_, std::forward<U>(element));
    size_++;
    write_ind_=(write_ind_+1)%capacity_;
}


template<typename T, bool Extendable, typename Allocator>
template<typename U>
void circular_buffer<T, Extendable, Allocator>::push_front(U&& element){

    if(size_==capacity_){
        Extend();
    }

    if(read_ind_==0){
        read_ind_=capacity_-1;
    }
    else{
        read_ind_=read_ind_-1;
    }
    std::allocator_traits<Allocator>::construct(alloc_, data_+read_ind_, std::forward<U>(element));
    size_++;
}


template<typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::pop_front(){

    std::allocator_traits<Allocator>::destroy(alloc_, data_+read_ind_);
    read_ind_=(read_ind_+1)%capacity_;
}

template<typename T, bool Extendable, typename Allocator>
void circular_buffer<T, Extendable, Allocator>::pop_back(){

    if(write_ind_==0){
        write_ind_=capacity_-1;
    }
    else{
        write_ind_=write_ind_-1;
    }

    std::allocator_traits<Allocator>::destroy(alloc_, data_+write_ind_);
}
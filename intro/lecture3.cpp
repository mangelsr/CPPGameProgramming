#include <iostream>

void pp(int &i)
{
    std::cout << &i << " " << i << " " << sizeof(i) << "\n";
}

template <typename T> // we can also use class keyword instead of typename
class DynamicArray
{
    size_t m_size;
    T *m_array;

public:
    // Constructor
    // Same name as the class
    DynamicArray(size_t size) : m_size(size), m_array(new T[size]) // new keyword initialize on 0
    {
        std::cout << "Array Constructor" << "\n";
    }

    // Destructor
    // Same name as the class
    // ~ tilde character at the begining
    ~DynamicArray()
    {
        delete[] m_array;
        std::cout << "Array Destructor" << "\n";
    }

    T get(size_t index) const
    {
        return m_array[index];
    }

    void set(size_t index, T value)
    {
        m_array[index] = value;
    }

    void print() const
    {
        for (size_t i = 0; i < m_size; i++)
        {
            std::cout << i << ": " << m_array[i] << "\n";
        }
    }

    // Operator overloading
    // square bracket operator
    const T &operator[](size_t index) const
    {
        return m_array[index];
    }

    T &operator[](size_t index)
    {
        return m_array[index];
    }

    // We can have both versions of the function
    // A const version for retreving data
    // And a non const version for updating data
};

int main(int argc, char *argv[])
{
    int a = 10;
    int b = 25;

    std::cout << "Simple variables allocated in stack" << "\n";

    pp(a);
    pp(b);

    std::cout << "\n";

    // Stack allocated array
    std::cout << "Stack Allocated Array" << "\n";

    // int arr[10];
    // Dont give default values

    int arr[10] = {}; // This syntax gives default values of 0

    for (size_t i = 0; i < 10; i++)
    {
        pp(arr[i]);
    }

    std::cout << "\n";

    // Heap allocated array
    std::cout << "Heap Allocated Array" << "\n";

    int *harr = new int[10];
    for (size_t i = 0; i < 10; i++)
    {
        pp(harr[i]);
    }

    std::cout << "\n";

    int c = 10;
    int d = 25;

    int *pc = &c;
    int *pd = &d;

    // *pc = 17; == *(&c) = 17;
    // *pc = 17;

    *(pd - 1) = 17; // Modify c value through a pointer to d
    // This is why raw pointers are insecure

    pp(c);
    pp(d);

    // pp(pc);
    // pp(pd);

    std::cout << "\n";

    std::cout << "RAII Int Array test" << "\n";

    DynamicArray<float> myArray(10);

    myArray.set(4, 7.65);
    myArray.set(2, 134.32);

    myArray[1] = 3.14;
    myArray[3] = 9.91;

    myArray.print();

    return 0;
}
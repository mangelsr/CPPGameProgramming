#include <iostream>
#include <vector>
#include <fstream>

class Student
{
    // Default values can be granted on the class declaration
    std::string m_first = "First";
    std::string m_last = "Last";
    int m_id = 0;
    float m_avg = 0;

public:
    // Default constructor
    Student() {}

    // Argument constructor
    Student(std::string first, std::string last, int id, float avg)
        : m_first(first), m_last(last), m_id(id), m_avg(avg)
    {
    }

    // Every single method that did not change the member variables should be marked as const
    // It's a good practice called const correctness

    int getAverage() const
    {
        return m_avg;
    }

    int getID() const
    {
        return m_id;
    }

    std::string getFullName() const
    {
        return m_first + " " + m_last;
    }

    // const functions will never change the object
    void print() const
    {
        std::cout << m_first << " " << m_last << " ";
        std::cout << m_id << " " << m_avg << "\n";
    }
};

class Course
{
    std::string m_name = "Course";
    std::vector<Student> m_students;

public:
    Course() {}

    Course(const std::string &name) : m_name(name) {}

    void addStudent(const Student &s)
    {
        m_students.push_back(s);
    }

    // the const at the beging of the method states that the returned value can't be changed
    // the const at the end of the method states that the method is not changing the object
    const std::vector<Student> &getStudents() const
    {
        return m_students;
    }

    void print() const
    {
        for (const auto &s : m_students)
        {
            s.print();
        }
    }

    void loadFromFile(const std::string &filename)
    {
        std::ifstream fileIn(filename);

        std::string first, last;
        int id;
        float avg;

        // >> get the next token on the stream
        while (fileIn >> first)
        {
            fileIn >> last >> id >> avg;
            m_students.push_back(Student(first, last, id, avg));
        }
    }
};

/*
Pass by reference it's a little bit safer
This example mentions that the Data object received is huge
So passing it by reference will be the optimal way
And mark it as const helps us to maintain it's integrity avoiding overrides
*/
// void doMachineLearning(const Data &d)
// {
// }

int main(int argc, char *argv[])
{
    // int age = 27;
    std::string first = "Miguel";
    std::string last = "Sanchez";
    std::string name = first + " " + last;

    // std::cout << name << " is " << age << std::endl;

    std::vector<int> vec;
    vec.push_back(24);
    vec.push_back(28);
    vec.push_back(56);

    // std::cout << vec[0] << "\n";
    // std::cout << vec[1] << "\n";
    // std::cout << vec[2] << "\n"; // Get whats is in that memory space

    // size_t safe datatype to use when iterating on vectors
    // for (size_t i = 0; i < vec.size(); i++)
    // {
    //     std::cout << vec[i] << "\n";
    // }

    // for (int a : vec)
    // {
    //     std::cout << a << "\n";
    // }

    std::vector<float> vec2;
    vec2.push_back(24.3f);
    vec2.push_back(28.2f);
    vec2.push_back(56.6f);

    // auto keyboard will infer the data type at compile time
    // auto => type inference
    // auto will copy the entire received object if its a Data Structure
    // for (auto a : vec2)
    // {
    //     std::cout << a << "\n";
    // }

    // Student s1;
    // Student s2("Miguel", "Sanchez", 243, 3.14);

    // const Student s3("Jane", "Doe", 20220001, 99.9);
    // const objects can only call const methods

    Course course("COMP4300 C++ Game Programming");
    // course.addStudent(s1);
    // course.addStudent(s2);
    // course.addStudent(s3);
    // course.addStudent(Student("Billy", "Bob", 3, 50.0));

    course.loadFromFile("students.txt");
    course.print();

    return 0;
}
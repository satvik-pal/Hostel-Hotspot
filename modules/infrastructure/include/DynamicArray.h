#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

// Resizable array (own replacement for std::vector).
// T must have a default constructor and copy assignment.

template <typename T>
class DynamicArray
{
    private:
        T* data;            // the elements
        int capacity;       // size of the allocated block
        int count;          // number of elements in use

        static constexpr int INITIAL_CAPACITY = 8;

        void grow()
        {
            int newCapacity = capacity * 2;
            T* newData = new T[newCapacity];

            for (int i = 0; i < count; i++)
            {
                newData[i] = data[i];
            }

            delete[] data;
            data = newData;
            capacity = newCapacity;
        }

    public:
        // Empty array.
        DynamicArray()
            : data(new T[INITIAL_CAPACITY]),
              capacity(INITIAL_CAPACITY),
              count(0)
        {
        }

        // Array of `size` elements, all set to `fillValue`.
        DynamicArray(int size, const T& fillValue)
            : data(nullptr),
              capacity(size > 0 ? size : INITIAL_CAPACITY),
              count(size > 0 ? size : 0)
        {
            data = new T[capacity];
            for (int i = 0; i < count; i++)
            {
                data[i] = fillValue;
            }
        }

        // Copy constructor: deep copy (new block, every element copied).
        DynamicArray(const DynamicArray& other)
            : data(new T[other.capacity]),
              capacity(other.capacity),
              count(other.count)
        {
            for (int i = 0; i < count; i++)
            {
                data[i] = other.data[i];
            }
        }

        // Copy assignment: replace this array's contents with a deep copy of `other`.
        DynamicArray& operator=(const DynamicArray& other)
        {
            if (this == &other)
            {
                return *this;                   // a = a: nothing to do
            }

            T* newData = new T[other.capacity];
            for (int i = 0; i < other.count; i++)
            {
                newData[i] = other.data[i];
            }

            delete[] data;                      // free the old block only after copying succeeded
            data = newData;
            capacity = other.capacity;
            count = other.count;
            return *this;
        }

        ~DynamicArray()
        {
            delete[] data;
        }

        // Adds an element at the end, growing if needed.
        void push(const T& value)
        {
            if (count == capacity)
            {
                grow();
            }
            data[count] = value;
            count++;
        }

        // Element access. Like a normal array, the index is NOT checked:
        // it must be between 0 and size() - 1.
        T& operator[](int index)
        {
            return data[index];
        }

        const T& operator[](int index) const
        {
            return data[index];
        }

        int size() const
        {
            return count;
        }

        bool isEmpty() const
        {
            return count == 0;
        }

        // Reverses the order of the elements in place.
        void reverse()
        {
            int left = 0;
            int right = count - 1;

            while (left < right)
            {
                T temp = data[left];
                data[left] = data[right];
                data[right] = temp;

                left++;
                right--;
            }
        }

        // Removes all elements (keeps the allocated block).
        void clear()
        {
            count = 0;
        }
};

#endif

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

// Binary min-heap stored in a dynamic array.
// The smallest item (according to operator<) is always at the top.
// T must have a default constructor, copy assignment and operator<.
template <typename T>
class MinHeap
{
    private:
        T* data;            // heap items, data[0] is the smallest
        int capacity;       // size of the allocated array
        int count;          // number of items currently stored

        static constexpr int INITIAL_CAPACITY = 16;

        static int parentOf(int i)     { return (i - 1) / 2; }
        static int leftChildOf(int i)  { return 2 * i + 1; }
        static int rightChildOf(int i) { return 2 * i + 2; }

        void swapItems(int a, int b)
        {
            T temp = data[a];
            data[a] = data[b];
            data[b] = temp;
        }

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

        // Move the item at index i up until its parent is not larger.
        void siftUp(int i)
        {
            while (i > 0 && data[i] < data[parentOf(i)])
            {
                swapItems(i, parentOf(i));
                i = parentOf(i);
            }
        }

        // Move the item at index i down until both children are not smaller.
        void siftDown(int i)
        {
            while (true)
            {
                int left = leftChildOf(i);
                int right = rightChildOf(i);
                int smallest = i;

                if (left < count && data[left] < data[smallest])
                {
                    smallest = left;
                }
                if (right < count && data[right] < data[smallest])
                {
                    smallest = right;
                }
                if (smallest == i)
                {
                    return;                 // heap property restored
                }

                swapItems(i, smallest);
                i = smallest;
            }
        }

    public:
        MinHeap()
            : data(new T[INITIAL_CAPACITY]),
              capacity(INITIAL_CAPACITY),
              count(0)
        {
        }

        ~MinHeap()
        {
            delete[] data;
        }

        // Copying is disabled: a shallow copy would make two heaps
        // share (and both delete) the same array.
        MinHeap(const MinHeap&) = delete;
        MinHeap& operator=(const MinHeap&) = delete;

        void push(const T& item)
        {
            if (count == capacity)
            {
                grow();
            }

            data[count] = item;             // place at the end
            siftUp(count);                  // restore heap order
            count++;
        }

        // Copies the smallest item into `out` without removing it.
        // Returns false if the heap is empty.
        bool peek(T& out) const
        {
            if (count == 0)
            {
                return false;
            }
            out = data[0];
            return true;
        }

        // Removes the smallest item and copies it into `out`.
        // Returns false if the heap is empty.
        bool pop(T& out)
        {
            if (count == 0)
            {
                return false;
            }

            out = data[0];
            count--;

            if (count > 0)
            {
                data[0] = data[count];      // move last item to the top
                siftDown(0);                // restore heap order
            }
            return true;
        }

        bool isEmpty() const
        {
            return count == 0;
        }

        int size() const
        {
            return count;
        }
};

#endif
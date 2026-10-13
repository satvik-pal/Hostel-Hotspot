#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <string>

// Hash table with separate chaining.
// Key: std::string.  Value: any type V.
template <typename V>
class HashMap
{
    private:
        struct Node
        {
            std::string key;
            V value;
            Node* next;

            Node(const std::string& k, const V& v, Node* n)
                : key(k), value(v), next(n)
            {
            }
        };

        Node** buckets;     // array of linked-list heads
        int capacity;       // number of buckets
        int count;          // number of stored entries

        static constexpr int INITIAL_CAPACITY = 16;

        static unsigned long hashString(const std::string& key)
        {
            unsigned long hash = 5381;                   // djb2
            for (char c : key)
            {
                hash = hash * 33 + static_cast<unsigned char>(c);
            }
            return hash;
        }

        int bucketIndex(const std::string& key) const
        {
            return static_cast<int>(hashString(key) % capacity);
        }

        void rehash()
        {
            int oldCapacity = capacity;
            Node** oldBuckets = buckets;

            capacity = oldCapacity * 2;
            buckets = new Node*[capacity];
            for (int i = 0; i < capacity; i++)
            {
                buckets[i] = nullptr;
            }

            // Move every node into its new bucket (no copying, just relinking)
            for (int i = 0; i < oldCapacity; i++)
            {
                Node* current = oldBuckets[i];
                while (current != nullptr)
                {
                    Node* nextNode = current->next;

                    int index = bucketIndex(current->key);
                    current->next = buckets[index];
                    buckets[index] = current;

                    current = nextNode;
                }
            }

            delete[] oldBuckets;
        }

    public:
        HashMap()
            : buckets(new Node*[INITIAL_CAPACITY]),
              capacity(INITIAL_CAPACITY),
              count(0)
        {
            for (int i = 0; i < capacity; i++)
            {
                buckets[i] = nullptr;
            }
        }

        ~HashMap()
        {
            for (int i = 0; i < capacity; i++)
            {
                Node* current = buckets[i];
                while (current != nullptr)
                {
                    Node* nextNode = current->next;
                    delete current;
                    current = nextNode;
                }
            }
            delete[] buckets;
        }

        // Copying is disabled: a shallow copy would make two maps
        // share (and both delete) the same nodes.
        HashMap(const HashMap&) = delete;
        HashMap& operator=(const HashMap&) = delete;

        // Adds key -> value. Returns false if the key already exists.
        bool insert(const std::string& key, const V& value)
        {
            if (contains(key))
            {
                return false;
            }

            // Keep load factor (count / capacity) at or below 0.75
            if ((count + 1) * 4 > capacity * 3)
            {
                rehash();
            }

            int index = bucketIndex(key);
            buckets[index] = new Node(key, value, buckets[index]);   // insert at head
            count++;
            return true;
        }

        // Returns a pointer to the stored value, or nullptr if not found.
        V* find(const std::string& key)
        {
            Node* current = buckets[bucketIndex(key)];
            while (current != nullptr)
            {
                if (current->key == key)
                {
                    return &current->value;
                }
                current = current->next;
            }
            return nullptr;
        }

        const V* find(const std::string& key) const
        {
            Node* current = buckets[bucketIndex(key)];
            while (current != nullptr)
            {
                if (current->key == key)
                {
                    return &current->value;
                }
                current = current->next;
            }
            return nullptr;
        }

        bool contains(const std::string& key) const
        {
            return find(key) != nullptr;
        }

        // Removes key. Returns false if it was not found.
        bool erase(const std::string& key)
        {
            int index = bucketIndex(key);
            Node* current = buckets[index];
            Node* previous = nullptr;

            while (current != nullptr)
            {
                if (current->key == key)
                {
                    if (previous == nullptr)
                    {
                        buckets[index] = current->next;      // removing the head
                    }
                    else
                    {
                        previous->next = current->next;      // unlink from middle/end
                    }
                    delete current;
                    count--;
                    return true;
                }
                previous = current;
                current = current->next;
            }
            return false;
        }

        int size() const
        {
            return count;
        }

        // Calls action(key, value) for every entry.
        template <typename Action>
        void forEach(Action action)
        {
            for (int i = 0; i < capacity; i++)
            {
                for (Node* current = buckets[i]; current != nullptr; current = current->next)
                {
                    action(current->key, current->value);
                }
            }
        }
};

#endif
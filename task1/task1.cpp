#include <algorithm>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>


template<class KeyType, class ValueType, class Hash = std::hash<KeyType> >
class HashMap {
private:
    std::vector<std::list<std::pair<const KeyType, ValueType>>> conteiner;
    size_t num_elements = 0;
    Hash hasher;
    void realoc(size_t new_size) {
        if (new_size < 1) return;
        std::vector<std::list<std::pair<const KeyType, ValueType>>> old_conteiner;
        std::swap(conteiner, old_conteiner);
        conteiner.resize(new_size);
        for (auto& cur_list : old_conteiner) {
            for (auto& object : cur_list) {
                size_t new_pos = hasher(object.first) % (new_size);
                conteiner[new_pos].push_back(object);
            }
        }
    }
public:
    class iterator {
    private:
        std::pair<const KeyType, ValueType>* value;
        std::vector<std::list<std::pair<const KeyType, ValueType>>>* table;
        Hash* hasher;
    public:
        iterator() : value(nullptr) {}
        iterator(const iterator& other) {
            value = other.value;
            table = other.table;
            hasher = other.hasher;
        }
        iterator(std::pair<const KeyType, ValueType>* other) {
            this->value = other;
        }
        iterator(std::vector<std::list<std::pair<const KeyType, ValueType>>>* _table,
            Hash* _hasher, std::pair<const KeyType, ValueType>* _value = nullptr) {
            value = _value;
            table = _table;
            hasher = _hasher;
        }
        iterator operator =(const iterator& other) {
            value = other.value;
            table = other.table;
            hasher = other.hasher;
            return *this;
        }
        bool operator ==(const iterator& other) const {
            return (other.value == value && other.table == table);
        }
        bool operator !=(const iterator& other) const {
            return !(other == *this);
        }
        iterator& operator ++() {
            size_t pos = (*hasher)(value->first) % ((*table).size());
            bool was = false;
            for (auto& object : (*table)[pos]) {
                if (&object == value) {
                    was = true;
                }
                else if (was) {
                    value = &object;
                    was = false;
                    break;
                }
            }
            if (was) {
                for (size_t other_pos = pos + 1; other_pos < (*table).size(); ++other_pos) {
                    auto& cur_list = (*table)[other_pos];
                    if (!cur_list.empty()) {
                        value = &(*cur_list.begin());
                        was = false;
                        break;
                    }
                }
            }
            if (was) {
                value = nullptr;
            }
            return *this;
        }
        iterator operator ++(int) {
            iterator old_value = *this;
            ++(*this);
            return old_value;
        }
        std::pair<const KeyType, ValueType> operator *() const {
            return *value;
        }
        std::pair<const KeyType, ValueType>* operator ->() const {
            return value;
        }
    };

    class const_iterator {
    private:
        const std::pair<const KeyType, ValueType>* value;
        const std::vector<std::list<std::pair<const KeyType, ValueType>>>* table;
        const Hash* hasher;
    public:
        const_iterator() : value(nullptr) {}
        const_iterator(const const_iterator& other) {
            value = other.value;
            table = other.table;
            hasher = other.hasher;
        }
        const_iterator(const std::pair<const KeyType, ValueType>* other) {
            this->value = other;
        }
        const_iterator(const std::vector<std::list<std::pair<const KeyType, ValueType>>>* _table,
            const Hash* _hasher, const std::pair<const KeyType, ValueType>* _value = nullptr) {
            value = _value;
            table = _table;
            hasher = _hasher;
        }
        const_iterator operator =(const const_iterator& other) {
            value = other.value;
            table = other.table;
            hasher = other.hasher;
            return *this;
        }
        bool operator ==(const const_iterator& other) const {
            return (other.value == value && other.table == table);
        }
        bool operator !=(const const_iterator& other) const {
            return !(other == *this);
        }
        const_iterator& operator ++() {
            size_t pos = (*hasher)(value->first) % ((*table).size());
            bool was = false;
            for (auto& object : (*table)[pos]) {
                if (&object == value) {
                    was = true;
                }
                else if (was) {
                    value = &object;
                    was = false;
                    break;
                }
            }
            if (was) {
                for (size_t other_pos = pos + 1; other_pos < (*table).size(); ++other_pos) {
                    auto& cur_list = (*table)[other_pos];
                    if (!cur_list.empty()) {
                        value = &(*cur_list.begin());
                        was = false;
                        break;
                    }
                }
            }
            if (was) {
                value = nullptr;
            }
            return *this;
        }
        const_iterator operator ++(int) {
            const_iterator old_value = *this;
            ++(*this);
            return old_value;
        }
        const std::pair<const KeyType, ValueType> operator *() const {
            return *value;
        }
        const std::pair<const KeyType, ValueType>* operator ->() const {
            return value;
        }
    };
    iterator begin() {
        for (auto& cur_list : conteiner) {
            if (!cur_list.empty()) {
                return iterator(&conteiner, &hasher, &(*cur_list.begin()));
            }
        }
        return iterator(&conteiner, &hasher);
    }
    const_iterator begin() const {
        for (auto& cur_list : conteiner) {
            if (!cur_list.empty()) {
                return const_iterator(&conteiner, &hasher, &(*cur_list.begin()));
            }
        }
        return const_iterator(&conteiner, &hasher);
    }
    iterator end() {
        return iterator(&conteiner, &hasher);
    }
    const_iterator end() const {
        return const_iterator(&conteiner, &hasher);
    }

    HashMap(Hash _hasher = Hash()) : conteiner(1), hasher(_hasher) {}
    template<class Iterator>
    HashMap(Iterator first, Iterator last, Hash _hasher = Hash()) : conteiner(1), hasher(_hasher) {
        while (first != last) {
            operator[](first->first) = first->second;
            ++first;
        }
    }
    HashMap(const std::initializer_list<std::pair<const KeyType, ValueType>>& other,
        Hash _hasher = Hash()) : conteiner(1), hasher(_hasher) {
        auto* first = other.begin();
        while (first != other.end()) {
            operator[](first->first) = first->second;
            ++first;
        }
    }
    size_t size() const {
        return num_elements;
    }
    bool empty() const {
        return num_elements == 0;
    }
    Hash hash_function() const {
        return hasher;
    }
    void insert(const std::pair<const KeyType, ValueType>& element) {
        size_t pos = hasher(element.first) % (conteiner.size());
        bool was = false;
        for (auto& object : conteiner[pos]) {
            if (object.first == element.first) {
                was = true;
            }
        }
        if (!was) {
            conteiner[pos].push_back(element);
            ++num_elements;
            if (num_elements > conteiner.size()) {
                realoc(conteiner.size() * 2);
            }
        }
    }
    void erase(const KeyType& key) {
        size_t pos = hasher(key) % (conteiner.size());
        auto object = conteiner[pos].begin();
        while (object != conteiner[pos].end()) {
            if (object->first == key) {
                conteiner[pos].erase(object);
                --num_elements;
                if (num_elements < conteiner.size() / 4) {
                    realoc(conteiner.size() / 2 + 1);
                }
                break;
            }
            ++object;
        }
    }
    iterator find(const KeyType& key) {
        size_t pos = hasher(key) % (conteiner.size());
        auto object = conteiner[pos].begin();
        while (object != conteiner[pos].end()) {
            if (object->first == key) {
                return iterator(&conteiner, &hasher, &(*object));
            }
            ++object;
        }
        return iterator(&conteiner, &hasher);
    }
    const_iterator find(const KeyType& key) const {
        size_t pos = hasher(key) % (conteiner.size());
        auto object = conteiner[pos].begin();
        while (object != conteiner[pos].end()) {
            if (object->first == key) {
                return const_iterator(&conteiner, &hasher, &(*object));
            }
            ++object;
        }
        return const_iterator(&conteiner, &hasher);
    }
    ValueType& operator [](const KeyType& key) {
        size_t pos = hasher(key) % (conteiner.size());
        auto object = conteiner[pos].begin();
        while (object != conteiner[pos].end()) {
            if (object->first == key) {
                return (object->second);
            }
            ++object;
        }
        ++num_elements;
        if (num_elements > conteiner.size()) {
            realoc(conteiner.size() * 2);
        }
        pos = hasher(key) % (conteiner.size());
        conteiner[pos].push_front(std::make_pair(key, ValueType()));
        return (conteiner[pos].begin())->second;
    }
    const ValueType& at(const KeyType& key) const {
        size_t pos = hasher(key) % (conteiner.size());
        auto object = conteiner[pos].begin();
        while (object != conteiner[pos].end()) {
            if (object->first == key) {
                return (object->second);
            }
            ++object;
        }
        throw std::out_of_range("no such key");
    }
    void clear() {
        conteiner.clear();
        num_elements = 0;
        conteiner.resize(1);
    }
    HashMap operator =(const HashMap& other) {
        this->clear();
        auto first = other.begin();
        auto last = other.end();
        while (first != last) {
            operator[](first->first) = first->second;
            ++first;
        }
        return *this;
    }
};
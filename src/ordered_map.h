#pragma once

#pragma once

#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <functional>

namespace scidf
{   
    template <typename key_t, typename val_t, typename idx_t>
    struct ordered_map_iterator
    {
        using base_iter_type = typename std::map<idx_t, key_t>::iterator;
        base_iter_type ordered_iter;
        std::map<key_t, val_t>& values_ref;
        std::unique_ptr<std::pair<key_t&, val_t&>> cur = nullptr;
        
        std::pair<key_t&, val_t&>& operator*()
        {
            key_t& key_out = (*ordered_iter).second;
            val_t& val_out = values_ref.at(key_out);
            std::pair<key_t&, val_t&> out{key_out, val_out};
            cur = std::make_unique<std::pair<key_t&, val_t&>>(out);
            return *cur;
        }
        
        bool operator ==(const ordered_map_iterator& rhs) const
        {
            return (rhs.ordered_iter == ordered_iter) && (&values_ref == &(rhs.values_ref));
        }
        
        ordered_map_iterator& operator ++()
        {
            ++ordered_iter;
            return *this;
        }
    };
    
    template <typename key_t, typename val_t, typename idx_t>
    struct const_ordered_map_iterator
    {
        using base_iter_type = typename std::map<idx_t, key_t>::const_iterator;
        base_iter_type ordered_iter;
        const std::map<key_t, val_t>& values_ref;
        
        std::pair<const key_t&, const val_t&> operator*() const
        {
            const key_t& key_out = (*ordered_iter).second;
            const val_t& val_out = values_ref.at(key_out);
            return std::pair<const key_t&, const val_t&>{key_out, val_out};
        }
        
        bool operator ==(const const_ordered_map_iterator& rhs) const
        {
            return (rhs.ordered_iter == ordered_iter) && (&values_ref == &(rhs.values_ref));
        }
        
        const_ordered_map_iterator& operator ++()
        {
            ++ordered_iter;
            return *this;
        }
    };
    
    template <typename key_t, typename val_t>
    class ordered_map
    {
        using index_type = std::size_t;
        using key_type   = key_t;
        using value_type = val_t;
        
        using iterator_type       = ordered_map_iterator<key_t, val_t, index_type>;
        using const_iterator_type = const_ordered_map_iterator<key_t, val_t, index_type>;
        
        public:
        ordered_map() : count{0} {}
        
        std::size_t size() const { return values.size(); }
        
        val_t& at(const key_t& key)
        {
            return values.at(key);
        }
        
        const val_t& at(const key_t& key) const
        {
            return values.at(key);
        }
        
        val_t& operator [] (const key_t& key)
        {
            if (this->find(key) == this->end()) this->insert({key, val_t()});
            return this->at(key);
        }
        
        void insert(const std::pair<key_t, val_t>& p)
        {
            auto idx = this->get_next_index();
            idx_to_key.insert({idx, p.first});
            key_to_idx.insert({p.first, idx});
            values.insert(p);
        }
        
        void insert(std::pair<key_t, val_t>&& p)
        {
            auto idx = this->get_next_index();
            idx_to_key.insert({idx, p.first});
            key_to_idx.insert({p.first, idx});
            values.emplace(std::move(p));
        }
        
        iterator_type begin()
        {
            return ordered_map_iterator<key_t, val_t, index_type>{idx_to_key.begin(), values};
        }
        
        iterator_type end()
        {
            return ordered_map_iterator<key_t, val_t, index_type>{idx_to_key.end(), values};
        }
        
        const_iterator_type begin() const
        {
            return const_ordered_map_iterator<key_t, val_t, index_type>{idx_to_key.begin(), values};
        }
        
        const_iterator_type end() const
        {
            return const_ordered_map_iterator<key_t, val_t, index_type>{idx_to_key.end(), values};
        }
        
        iterator_type find(const key_type& key)
        {
            if (key_to_idx.find(key) == key_to_idx.end()) return this->end();
            return ordered_map_iterator<key_t, val_t, index_type>{idx_to_key.find(key_to_idx.at(key)), values};
        }
        
        const_iterator_type find(const key_type& key) const
        {
            if (key_to_idx.find(key) == key_to_idx.end()) return this->end();
            return const_ordered_map_iterator<key_t, val_t, index_type>{idx_to_key.find(key_to_idx.at(key)), values};
        }
        
        void erase(const iterator_type& key)
        {
            const auto& keyval = (*(key.ordered_iter)).second;
            const auto  idx    = (*(key.ordered_iter)).first;
            key_to_idx.erase(keyval);
            idx_to_key.erase(idx);
            values.erase(keyval);
        }
        
        void erase(const const_iterator_type& key)
        {
            const auto& keyval = (*(key.ordered_iter)).second;
            const auto  idx    = (*(key.ordered_iter)).first;
            key_to_idx.erase(keyval);
            idx_to_key.erase(idx);
            values.erase(keyval);
        }
        
        private:
        index_type get_next_index()
        {
            return count++;
        }
        
        index_type                     count;
        std::map<key_type, index_type> key_to_idx;
        std::map<index_type, key_type> idx_to_key;
        std::map<key_type, value_type> values;
    };
}
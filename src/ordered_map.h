#pragma once

#include <cstddef>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <functional>
#include <list>
#include <stdexcept>

//namespace cui::util
//{
    // An ordered_map that preserves insertion order without proxies
    // Internally stores elements in a linked list and indexes them in a map.
    template <typename key_t, typename val_t>
    class ordered_map
    {
        public:
        using value_type      = std::pair<const key_t, val_t>;
        using list_type       = std::list<value_type>;
        using iterator        = typename list_type::iterator;
        using const_iterator  = typename list_type::const_iterator;
        using size_type       = std::size_t;

        ordered_map() = default;

        bool empty() const noexcept { return seq.empty(); }
        size_type size() const noexcept { return seq.size(); }

        val_t& at(const key_t& key)
        {
            auto it = lookup.find(key);
            if (it == lookup.end())
                throw std::out_of_range("ordered_map::at: key not found");
            return it->second->second;
        }

        const val_t& at(const key_t& key) const
        {
            auto it = lookup.find(key);
            if (it == lookup.end())
                throw std::out_of_range("ordered_map::at: key not found");
            return it->second->second;
        }

        val_t& operator[](const key_t& key)
        {
            auto it = lookup.find(key);
            if (it == lookup.end()) {
                seq.emplace_back(key, val_t{});
                auto lit = std::prev(seq.end());
                lookup[lit->first] = lit;
                return lit->second;
            }
            return it->second->second;
        }

        void insert(const value_type& p)
        {
            if (lookup.count(p.first)) return;  // ignore duplicates
            seq.emplace_back(p);
            auto lit = std::prev(seq.end());
            lookup[lit->first] = lit;
        }

        void insert(value_type&& p)
        {
            if (lookup.count(p.first)) return;
            seq.emplace_back(std::move(p));
            auto lit = std::prev(seq.end());
            lookup[lit->first] = lit;
        }

        iterator begin() noexcept       { return seq.begin(); }
        iterator end() noexcept         { return seq.end(); }
        const_iterator begin() const noexcept { return seq.begin(); }
        const_iterator end() const noexcept   { return seq.end(); }

        iterator find(const key_t& key)
        {
            auto it = lookup.find(key);
            return it != lookup.end() ? it->second : seq.end();
        }

        const_iterator find(const key_t& key) const
        {
            auto it = lookup.find(key);
            return it != lookup.end() ? it->second : seq.end();
        }

        void erase(const key_t& key)
        {
            auto it = lookup.find(key);
            if (it == lookup.end()) return;
            seq.erase(it->second);
            lookup.erase(it);
        }

        void erase(iterator pos)
        {
            lookup.erase(pos->first);
            seq.erase(pos);
        }

        void clear() noexcept
        {
            seq.clear();
            lookup.clear();
        }

        private:
        list_type                  seq;
        std::map<key_t, iterator> lookup;
    };
//}

/*

#pragma once


#include <cstdlib>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <functional>

namespace cui::util
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
        
        bool empty() const { return this->size() == 0; }
        
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
        
        void erase(const key_type& key)
        {
            this->erase(this->find(key));
        }
        
        void erase(const iterator_type& it)
        {
            // const auto& keyval = (*(key.ordered_iter)).second;
            // const auto  idx    = (*(key.ordered_iter)).first;
            // key_to_idx.erase(keyval);
            // idx_to_key.erase(idx);
            // values.erase(keyval);
            
            // grab both pieces before you start erasing
            const auto& keyval = it.ordered_iter->second;
            const auto  idx    = it.ordered_iter->first;
        
            // erase from the value‐map and the reverse‐index *before* nuking idx_to_key
            values.erase(keyval);
            key_to_idx.erase(keyval);
            idx_to_key.erase(idx);
        }
        
        void erase(const const_iterator_type& it)
        {
            const auto& keyval = it.ordered_iter->second;
            const auto  idx    = it.ordered_iter->first;
        
            // erase from the value‐map and the reverse‐index *before* nuking idx_to_key
            values.erase(keyval);
            key_to_idx.erase(keyval);
            idx_to_key.erase(idx);
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
*/

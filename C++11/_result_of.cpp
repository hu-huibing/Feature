#include <type_traits>
#include <algorithm>
#include <map>
#include <vector>
#include <iostream>

template<typename R>
class CRange
{
public:
    typedef typename R::value_type value_type;
    CRange(R &v):m_container(v){};

public:
    template<typename KeyFn, typename ValFn>
    std::multimap<typename std::result_of<KeyFn(value_type)>::type, typename std::result_of<ValFn(value_type)>::type>
    group_by(const KeyFn key_fn, const ValFn val_fn){
        using key_type = typename std::result_of<KeyFn(value_type)>::type;
        using val_type = typename std::result_of<ValFn(value_type)>::type;
        std::multimap<key_type, val_type> mm;
        std::for_each(std::begin(m_container), std::end(m_container), [&key_fn, &val_fn, &mm](const value_type &items){
            key_type key = key_fn(items);
            val_type value = val_fn(items);
            mm.insert(std::make_pair(key, value));
        });
        return mm;
    }

private:
    R   m_container;
};

/*----test----*/
struct Person {
    std::string name;
    int age;
    std::string city;
};

int main(void)
{
    std::vector<Person> person = {{"a", 1, "aa"}, {"b", 2, "bb"}, {"c", 3, "cc"}};
    CRange<std::vector<Person>> range(person);
    auto group_by_name_age_key = [](const Person& person){
        return std::tie(person.name, person.age);
    };
    auto group_by_name_age_value = [](const Person& person){
        return person.city;
    };
    auto mm = range.group_by(group_by_name_age_key, group_by_name_age_value);
    for (const auto &it : mm){
        std::cout << "_____________" << std::endl;
        std::cout << std::get<0>(it.first) << std::get<1>(it.first) << std::endl;
        std::cout << it.second << std::endl;
    }
    return 0;
}
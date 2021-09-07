#include <iostream>
#include <string>
#include <vector>

template <class T>
void print_vector(const std::vector<T>& input_vector) {
    for (std::size_t i=0; i<input_vector.size(); ++i) {
        std::cout << std::to_string(input_vector[i]);
        if (i < (input_vector.size() - 1)) {
            std::cout << ", ";
        }
    }
}

template <class T>
std::vector<T> read_vector(const std::string& input_string) {
    std::vector<T> output;
    std::string::size_type data_start = input_string.find("{");
    std::string::size_type data_end = input_string.find("}");

    std::string::size_type comma_pos;
    std::string::size_type next_comma_pos;

    bool hit_end = false;
    comma_pos = data_start;
    while (!hit_end) {
        next_comma_pos = input_string.find(",", comma_pos + 1);
        if (next_comma_pos > data_end) {
            hit_end = true;
            next_comma_pos = input_string.find("}", comma_pos);
        }
        std::string num = input_string.substr(comma_pos + 1, (next_comma_pos - 1 - comma_pos));
        if (num.substr(0, 1) == " ") {
            num = num.substr(1);
        }
        if (num.size()) {
            T value = static_cast<T>(std::stoll(num));
            output.push_back(value);
            comma_pos = next_comma_pos;
        }
    }

    return output;
}



template <class T>
class NullableArray {
    public:
        std::vector<T> data;
        std::vector<uint8_t> nulls;
    
        NullableArray<T> gather(const std::vector<uint32_t>& gather_map) const {
            NullableArray<T> result;
            int gather_vector_size = gather_map.size(); 
            int mod = NBITS;
            int N = (gather_vector_size + mod - 1) / mod; 
            result.nulls.resize(N); 
            for (size_t i = 0; i < gather_map.size(); i++) {
                int ind_out = i / NBITS; 
                int shift_out = i % NBITS;
                int ind_in = gather_map[i] / NBITS;
                int shift_in = gather_map[i] % NBITS;
                if (nulls[ind_in] & (ONE << shift_in)) { 
                    result.data.push_back(data[gather_map[i]]);
                    result.nulls[ind_out] |= (ONE << shift_out);
                } else {
                    result.data.push_back(0);
                    result.nulls[ind_out] &= ~(ONE << shift_out);
                }
            }
            return result;
        }
    private: 
    static const int ONE = 1;
    static const int NBITS = 8;
};

template <class T>
std::pair<NullableArray<T>, std::vector<uint32_t>> read_input_data(const std::string& input_string) {
    std::pair<NullableArray<T>, std::vector<uint32_t>> output;

    output.first.data = std::move(read_vector<T>(input_string));
    std::string::size_type data_end = input_string.find("}");
    auto split_string = input_string.substr(data_end + 2);
    output.first.nulls = std::move(read_vector<uint8_t>(split_string));
    std::string::size_type nulls_end = input_string.find("}", data_end + 2);
    split_string = input_string.substr(nulls_end + 2);
    output.second = std::move(read_vector<uint32_t>(split_string));
    return output;
}

template <class T>
void print_nullable_array(const NullableArray<T>& input) {
    std::cout << "data: {";
    print_vector(input.data);
    std::cout << "}" << std::endl;
    std::cout << "nulls: {";
    print_vector(input.nulls);
    std::cout << "}" << std::endl;
}

int main() {
    std::string input_data;
    std::getline(std::cin, input_data, static_cast<char>(EOF));
    std::pair<NullableArray<int32_t>, std::vector<uint32_t>> input_pair = read_input_data<int32_t>(input_data);
    const NullableArray<int32_t> my_nullable_array = std::move(input_pair.first);
    const std::vector<uint32_t> gather_map = std::move(input_pair.second);
    
    auto output_array = my_nullable_array.gather(gather_map);
    std::cout<< "Output Data" << std::endl << "===========" << std::endl;
    print_nullable_array<int32_t>(output_array);

    return 0;
}
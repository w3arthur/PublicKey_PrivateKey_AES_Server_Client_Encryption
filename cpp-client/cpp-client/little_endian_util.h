#pragma once
#include <cstdint>




template <typename T>
void to_little_endian(T& value, int size = sizeof(T)) {
    if constexpr (std::is_integral<T>::value) {
        ;
        T result = 0;

        for (int i = 0; i < size; ++i) {
            result |= (static_cast<T>(value & (0xFF << (i * 8))) >> (i * 8));
        }

        // Update the passed value in place
        value = result;
    }
    else if constexpr (std::is_pointer<T>::value) {
        // Assuming little-endian architecture
        using U = typename std::remove_pointer<T>::type;
        U temp;
        std::memcpy(&temp, value, sizeof(U));
        to_little_endian(temp);
        std::memcpy(value, &temp, sizeof(U));
    }
    else if constexpr (std::is_same<T, std::string>::value) {
        // Convert std::string to char* buffer
        std::string& str = value;
        char* buffer = &str[0];

        // Perform little-endian conversion on the buffer
        to_little_endian(buffer);

        // Update the original std::string from the modified buffer
        str.assign(buffer);
    }
    else {
        // Display a diagnostic message with the type information
        std::cerr << "Unsupported type: " << typeid(T).name() << std::endl;
    }
}
/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* HTTP Library: JSON parser and writer
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/
#pragma once

#ifndef _CASA_JSON_H
#define _CASA_JSON_H

#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "cpprest/details/basic_types.h"
#include "cpprest/asyncrt_utils.h"

namespace web
{
/// Library for parsing and serializing JSON values to and from C++ types.
namespace json
{
    // Various forward declarations.
    namespace details
    {
        class _Value;
        class _Number;
        class _Null;
        class _Boolean;
        class _UTF8String;
        class _Object;
        class _Array;
        class JSON_Parser;
    }

    namespace details
    {
        extern bool g_keep_json_object_unsorted;
    }

    /// <summary>
    /// Preserve the order of the name/value pairs when parsing a JSON object.
    /// The default is false, which can yield better performance.
    /// </summary>
    /// <param name="keep_order"><c>true</c> if ordering should be preserved when parsing, <c>false</c> otherwise.</param>
    /// <remarks>Note this is a global setting and affects all JSON parsing done.</remarks>
    void _ASYNCRTIMP __cdecl keep_object_element_order(bool keep_order);

#ifdef _WIN32
#ifdef _DEBUG
#define ENABLE_JSON_VALUE_VISUALIZER
#endif
#endif

    class number;
    class array;
    class object;

    /// <summary>
    /// A JSON value represented as a C++ class.
    /// </summary>
    class value
    {
    public:
        /// <summary>
        /// This enumeration represents the various kinds of JSON values.
        /// </summary>
        enum value_type
        {
            /// Number value
            Number,
            /// Boolean value
            Boolean,
            /// String value
            String,
            /// Object value
            Object,
            /// Array value
            Array,
            /// Null value
            Null
        };

        /// <summary>
        /// Constructor creating a null value
        /// </summary>
        _ASYNCRTIMP value();

        /// <summary>
        /// Constructor creating a JSON number value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from</param>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::number instead.")
        _ASYNCRTIMP value(int32_t value);

        /// <summary>
        /// Constructor creating a JSON number value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from</param>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::number instead.")
        _ASYNCRTIMP value(uint32_t value);

        /// <summary>
        /// Constructor creating a JSON number value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from</param>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::number instead.")
        _ASYNCRTIMP value(int64_t value);

        /// <summary>
        /// Constructor creating a JSON number value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from</param>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::number instead.")
        _ASYNCRTIMP value(uint64_t value);

        /// <summary>
        /// Constructor creating a JSON number value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from</param>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::number instead.")
        _ASYNCRTIMP value(double value);

        /// <summary>
        /// Constructor creating a JSON Boolean value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from</param>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::boolean instead.")
        _ASYNCRTIMP explicit value(bool value);

        /// <summary>
        /// Constructor creating a JSON string value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from, a C++ STL string of the platform-native character width</param>
        /// <remarks>
        /// This constructor has O(n) performance because it tries to determine if
        /// specified string has characters that should be properly escaped in JSON.
        /// </remarks>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::string instead.")
        _ASYNCRTIMP explicit value(utility::string_t value);

        /// <summary>
        /// Constructor creating a JSON string value specifying if the string contains characters to escape
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from, a C++ STL string of the platform-native character width</param>
        /// <param name="has_escape_chars">Whether <paramref name="value" /> contains characters
        /// that should be escaped in JSON value</param>
        /// <remarks>
        /// This constructor has O(1) performance if the string is move constructed.
        /// </remarks>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::string instead.")
        _ASYNCRTIMP explicit value(utility::string_t value, bool has_escape_chars);

        /// <summary>
        /// Constructor creating a JSON string value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from, a C++ STL string of the platform-native character width</param>
        /// <remarks>
        /// <para>
        /// This constructor has O(n) performance because it tries to determine if
        /// specified string has characters that should be properly escaped in JSON.
        /// </para>
        /// <para>
        /// This constructor exists in order to avoid string literals matching another constructor,
        /// as is very likely. For example, conversion to bool does not require a user-defined conversion,
        /// and will therefore match first, which means that the JSON value turns up as a boolean.
        /// </para>
        /// </remarks>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::string instead.")
        _ASYNCRTIMP explicit value(const utility::char_t* value);

        /// <summary>
        /// Constructor creating a JSON string value
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from, a C++ STL string of the platform-native character width</param>
        /// <param name="has_escape_chars">Whether <paramref name="value" /> contains characters
        /// that should be escaped in JSON value</param>
        /// <remarks>
        /// <para>
        /// This constructor has O(1) performance if the string is move constructed.
        /// </para>
        /// <para>
        /// This constructor exists in order to avoid string literals matching another constructor,
        /// as is very likely. For example, conversion to bool does not require a user-defined conversion,
        /// and will therefore match first, which means that the JSON value turns up as a boolean.
        /// </para>
        /// </remarks>
        CASABLANCA_DEPRECATED("This API is deprecated. Use the static factory function json::string instead.")
        _ASYNCRTIMP explicit value(const utility::char_t* value, bool has_escape_chars);

        /// <summary>
        /// Copy constructor
        /// </summary>
        _ASYNCRTIMP value(const value &);

        /// <summary>
        /// Move constructor
        /// </summary>
        _ASYNCRTIMP value(value &&) CPPREST_NOEXCEPT ;

        /// <summary>
        /// Assignment operator.
        /// </summary>
        /// <returns>The JSON value object that contains the result of the assignment.</returns>
        _ASYNCRTIMP value &operator=(const value &);

        /// <summary>
        /// Move assignment operator.
        /// </summary>
        /// <returns>The JSON value object that contains the result of the assignment.</returns>
        _ASYNCRTIMP value &operator=(value &&) CPPREST_NOEXCEPT ;

        // Static factories

        /// <summary>
        /// Creates a null value
        /// </summary>
        /// <returns>A JSON null value</returns>
        static _ASYNCRTIMP value __cdecl null();

        /// <summary>
        /// Creates a number value
        /// </summary>
        /// <param name="num">The C++ value to create a JSON value from</param>
        /// <returns>A JSON number value</returns>
        static _ASYNCRTIMP value __cdecl number(double num);

        /// <summary>
        /// Creates a number value
        /// </summary>
        /// <param name="num">The C++ value to create a JSON value from</param>
        /// <returns>A JSON number value</returns>
        static _ASYNCRTIMP value __cdecl number(int32_t num);

        /// <summary>
        /// Creates a number value
        /// </summary>
        /// <param name="num">The C++ value to create a JSON value from</param>
        /// <returns>A JSON number value</returns>
        static _ASYNCRTIMP value __cdecl number(uint32_t num);

        /// <summary>
        /// Creates a number value
        /// </summary>
        /// <param name="num">The C++ value to create a JSON value from</param>
        /// <returns>A JSON number value</returns>
        static _ASYNCRTIMP value __cdecl number(int64_t num);

        /// <summary>
        /// Creates a number value
        /// </summary>
        /// <param name="num">The C++ value to create a JSON value from</param>
        /// <returns>A JSON number value</returns>
        static _ASYNCRTIMP value __cdecl number(uint64_t num);

        /// <summary>
        /// Creates a Boolean value
        /// </summary>
        /// <param name="bval">The C++ value to create a JSON value from</param>
        /// <returns>A JSON Boolean value</returns>
        static _ASYNCRTIMP value __cdecl boolean(bool bval);

        /// <summary>
        /// Constructor creating a JSON string value
        /// </summary>
        /// <param name="str">The C++ value to create a JSON value from, a UTF-8 encoded C++ STL string</param>
        /// <remarks>
        /// This constructor has O(n) performance because it tries to determine if
        /// specified string has characters that should be properly escaped in JSON.
        /// <remarks>
        static _ASYNCRTIMP value __cdecl string(utf8string str);

        /// <summary>
        /// Constructor creating a JSON string value
        /// </summary>
        /// <param name="str">The C++ value to create a JSON value from, a UTF-16 encoded C++ STL string</param>
        /// <remarks>
        /// This constructor has O(n) performance because it tries to determine if
        /// specified string has characters that should be properly escaped in JSON.
        /// <remarks>
        static _ASYNCRTIMP value __cdecl string(const utf16string& str);

        /// <summary>
        /// Constructor creating a JSON string value specifying if the string contains characters to escape
        /// </summary>
        /// <param name="str">The C++ value to create a JSON value str, a UTF-8 encoded C++ STL string</param>
        /// <param name="has_escape_chars">Whether <paramref name="value" /> contains characters
        /// that should be escaped in JSON value</param>
        /// <remarks>
        /// This constructor has O(1) performance if the string is move constructed.
        /// </remarks>
        static _ASYNCRTIMP value __cdecl string(utf8string str, bool has_escape_chars);

        /// <summary>
        /// Constructor creating a JSON string value specifying if the string contains characters to escape
        /// </summary>
        /// <param name="str">The C++ value to create a JSON value from, a UTF-16 encoded C++ STL string</param>
        /// <param name="has_escape_chars">Whether <paramref name="str" /> contains characters
        /// that should be escaped in JSON value</param>
        /// <remarks>
        /// This constructor has O(1) performance if the string is move constructed.
        /// </remarks>
        static _ASYNCRTIMP value __cdecl string(const utf16string& str, bool has_escape_chars);

        /// <summary>
        /// Creates an object value
        /// </summary>
        /// <param name="keep_order">Whether to preserve the original order of the fields</param>
        /// <returns>An empty JSON object value</returns>
        static _ASYNCRTIMP json::value __cdecl object(bool keep_order = false);

        /// <summary>
        /// Creates an object value from a collection of field/values
        /// </summary>
        /// <param name="fields">Field names associated with JSON values</param>
        /// <param name="keep_order">Whether to preserve the original order of the fields</param>
        /// <returns>A non-empty JSON object value</returns>
        static _ASYNCRTIMP json::value __cdecl object(std::vector<std::pair<utf8string, value>> fields, bool keep_order = false);

        /// <summary>
        /// Creates an object value from a collection of field/values
        /// </summary>
        /// <param name="fields">Field names associated with JSON values</param>
        /// <param name="keep_order">Whether to preserve the original order of the fields</param>
        /// <returns>A non-empty JSON object value</returns>
        static _ASYNCRTIMP json::value __cdecl object(std::vector<std::pair<utf16string, value>> fields, bool keep_order = false);

        /// <summary>
        /// Creates an empty JSON array
        /// </summary>
        /// <returns>An empty JSON array value</returns>
        static _ASYNCRTIMP json::value __cdecl array();

        /// <summary>
        /// Creates a JSON array
        /// </summary>
        /// <param name="size">The initial number of elements of the JSON value</param>
        /// <returns>A JSON array value</returns>
        static _ASYNCRTIMP json::value __cdecl array(size_t size);

        /// <summary>
        /// Creates a JSON array
        /// </summary>
        /// <param name="elements">A vector of JSON values</param>
        /// <returns>A JSON array value</returns>
        static _ASYNCRTIMP json::value __cdecl array(std::vector<value> elements);

        /// <summary>
        /// Accesses the type of JSON value the current value instance is
        /// </summary>
        /// <returns>The value's type</returns>
        _ASYNCRTIMP json::value::value_type type() const;

        /// <summary>
        /// Is the current value a null value?
        /// </summary>
        /// <returns><c>true</c> if the value is a null value, <c>false</c> otherwise</returns>
        bool is_null() const { return type() == Null; };

        /// <summary>
        /// Is the current value a number value?
        /// </summary>
        /// <returns><c>true</c> if the value is a number value, <c>false</c> otherwise</returns>
        bool is_number() const { return type() == Number; }

        /// <summary>
        /// Is the current value represented as an integer number value?
        /// </summary>
        /// <remarks>
        /// Note that if a json value is a number but represented as a double it can still
        /// be retrieved as a integer using as_integer(), however the value will be truncated.
        /// </remarks>
        /// <returns><c>true</c> if the value is an integer value, <c>false</c> otherwise.</returns>
        _ASYNCRTIMP bool is_integer() const;

        /// <summary>
        /// Is the current value represented as an double number value?
        /// </summary>
        /// <remarks>
        /// Note that if a json value is a number but represented as a int it can still
        /// be retrieved as a double using as_double().
        /// </remarks>
        /// <returns><c>true</c> if the value is an double value, <c>false</c> otherwise.</returns>
        _ASYNCRTIMP bool is_double() const;

        /// <summary>
        /// Is the current value a Boolean value?
        /// </summary>
        /// <returns><c>true</c> if the value is a Boolean value, <c>false</c> otherwise</returns>
        bool is_boolean() const { return type() == Boolean; }

        /// <summary>
        /// Is the current value a string value?
        /// </summary>
        /// <returns><c>true</c> if the value is a string value, <c>false</c> otherwise</returns>
        bool is_string() const { return type() == String; }

        /// <summary>
        /// Is the current value an array?
        /// </summary>
        /// <returns><c>true</c> if the value is an array, <c>false</c> otherwise</returns>
        bool is_array() const { return type() == Array; }

        /// <summary>
        /// Is the current value an object?
        /// </summary>
        /// <returns><c>true</c> if the value is an object, <c>false</c> otherwise</returns>
        bool is_object() const { return type() == Object; }

        /// <summary>
        /// Gets the number of children of the value.
        /// </summary>
        /// <returns>The number of children. 0 for all non-composites.</returns>
        size_t size() const;

        /// <summary>
        /// Parses a string and construct a JSON value.
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from, a C++ STL double-byte string</param>
        _ASYNCRTIMP static value __cdecl parse(const utf8string& value);
        static inline value __cdecl parse(const utf16string& value) {
            return parse(utility::conversions::to_utf8string(value));
        }

        /// <summary>
        /// Attempts to parse a string and construct a JSON value.
        /// </summary>
        /// <param name="value">The C++ value to create a JSON value from, a C++ STL double-byte string</param>
        /// <param name="errorCode">If parsing fails, the error code is greater than 0</param>
        /// <returns>The parsed object. Returns web::json::value::null if failed</returns>
        _ASYNCRTIMP static value __cdecl parse(const utf8string& value, std::error_code &errorCode);
        static inline value __cdecl parse(const utf16string& value, std::error_code &errorCode) {
            return parse(utility::conversions::to_utf8string(value), errorCode);
        }

        /// <summary>
        /// Serializes the current JSON value to a C++ string.
        /// </summary>
        /// <returns>A string representation of the value</returns>
        _ASYNCRTIMP utility::string_t serialize() const;

        /// <summary>
        /// Serializes the current JSON value to a UTF-8 encoded C++ string.
        /// </summary>
        /// <returns>A utf8 string representation of the value</returns>
        _ASYNCRTIMP utf8string serialize_utf8() const;

        /// <summary>
        /// Serializes the current JSON value to a C++ string.
        /// </summary>
        /// <returns>A string representation of the value</returns>
        CASABLANCA_DEPRECATED("This API is deprecated and has been renamed to avoid confusion with as_string(), use ::web::json::value::serialize() instead.")
        _ASYNCRTIMP utility::string_t to_string() const;

        /// <summary>
        /// Parses a JSON value from the contents of an input stream using the native platform character width.
        /// </summary>
        /// <param name="input">The stream to read the JSON value from</param>
        /// <returns>The JSON value object created from the input stream.</returns>
        _ASYNCRTIMP static value __cdecl parse(utility::istream_t &input);

        /// <summary>
        /// Parses a JSON value from the contents of an input stream using the native platform character width.
        /// </summary>
        /// <param name="input">The stream to read the JSON value from</param>
        /// <param name="errorCode">If parsing fails, the error code is greater than 0</param>
        /// <returns>The parsed object. Returns web::json::value::null if failed</returns>
        _ASYNCRTIMP static value __cdecl parse(utility::istream_t &input, std::error_code &errorCode);

        /// <summary>
        /// Writes the current JSON value to a stream with the native platform character width.
        /// </summary>
        /// <param name="stream">The stream that the JSON string representation should be written to.</param>
        _ASYNCRTIMP void serialize(utility::ostream_t &stream) const;

#ifdef _WIN32
        /// <summary>
        /// Parses a JSON value from the contents of a single-byte (UTF8) stream.
        /// </summary>
        /// <param name="stream">The stream to read the JSON value from</param>
        _ASYNCRTIMP static value __cdecl parse(std::istream& stream);

        /// <summary>
        /// Parses a JSON value from the contents of a single-byte (UTF8) stream.
        /// </summary>
        /// <param name="stream">The stream to read the JSON value from</param>
        /// <param name="errorCode">If parsing fails, the error code is greater than 0</param>
        /// <returns>The parsed object. Returns web::json::value::null if failed</returns>
        _ASYNCRTIMP static value __cdecl parse(std::istream& stream, std::error_code& error);

        /// <summary>
        /// Serializes the content of the value into a single-byte (UTF8) stream.
        /// </summary>
        /// <param name="stream">The stream that the JSON string representation should be written to.</param>
        _ASYNCRTIMP void serialize(std::ostream& stream) const;
#endif

        /// <summary>
        /// Converts the JSON value to a C++ double, if and only if it is a number value.
        /// Throws <see cref="json_exception"/>  if the value is not a number
        /// </summary>
        /// <returns>A double representation of the value</returns>
        _ASYNCRTIMP double as_double() const;

        /// <summary>
        /// Converts the JSON value to a C++ integer, if and only if it is a number value.
        /// Throws <see cref="json_exception"/> if the value is not a number
        /// </summary>
        /// <returns>An integer representation of the value</returns>
        _ASYNCRTIMP int as_integer() const;

        /// <summary>
        /// Converts the JSON value to a number class, if and only if it is a number value.
        /// Throws <see cref="json_exception"/>  if the value is not a number
        /// </summary>
        /// <returns>An instance of number class</returns>
        _ASYNCRTIMP const json::number& as_number() const;

        /// <summary>
        /// Converts the JSON value to a C++ bool, if and only if it is a Boolean value.
        /// </summary>
        /// <returns>A C++ bool representation of the value</returns>
        _ASYNCRTIMP bool as_bool() const;

        /// <summary>
        /// Converts the JSON value to a json array, if and only if it is an array value.
        /// </summary>
        /// <remarks>The returned <c>json::array</c> should have the same or shorter lifetime as <c>this</c></remarks>
        /// <returns>An array representation of the value</returns>
        _ASYNCRTIMP json::array& as_array();

        /// <summary>
        /// Converts the JSON value to a json array, if and only if it is an array value.
        /// </summary>
        /// <remarks>The returned <c>json::array</c> should have the same or shorter lifetime as <c>this</c></remarks>
        /// <returns>An array representation of the value</returns>
        _ASYNCRTIMP const json::array& as_array() const;

        /// <summary>
        /// Converts the JSON value to a json object, if and only if it is an object value.
        /// </summary>
        /// <returns>An object representation of the value</returns>
        _ASYNCRTIMP json::object& as_object();

        /// <summary>
        /// Converts the JSON value to a json object, if and only if it is an object value.
        /// </summary>
        /// <returns>An object representation of the value</returns>
        _ASYNCRTIMP const json::object& as_object() const;

        /// <summary>
        /// Converts the JSON value to a C++ STL string, if and only if it is a string value.
        /// </summary>
        /// <returns>A C++ STL string representation of the value</returns>
        CASABLANCA_DEPRECATED("This API is deprecated. use as_utf8string() instead.")
        _ASYNCRTIMP utility::string_t as_string() const;

        /// <summary>
        /// Converts the JSON value to a C++ STL string, if and only if it is a string value.
        /// </summary>
        /// <returns>A C++ STL string representation of the value</returns>
        _ASYNCRTIMP const utf8string& as_utf8string() const;

        /// <summary>
        /// Compares two JSON values for equality.
        /// </summary>
        /// <param name="other">The JSON value to compare with.</param>
        /// <returns>True iff the values are equal.</returns>
        _ASYNCRTIMP bool operator==(const value& other) const;

        /// <summary>
        /// Compares two JSON values for inequality.
        /// </summary>
        /// <param name="other">The JSON value to compare with.</param>
        /// <returns>True iff the values are unequal.</returns>
        bool operator!=(const value& other) const
        {
            return !((*this) == other);
        }

        /// <summary>
        /// Tests for the presence of a field.
        /// </summary>
        /// <param name="key">The name of the field</param>
        /// <returns>True if the field exists, false otherwise.</returns>
        inline bool has_field(const utf8string& key) const;
        inline bool has_field(const utf16string& key) const
        {
            return has_field(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses a field of a JSON object.
        /// </summary>
        /// <param name="key">The name of the field</param>
        /// <returns>The value kept in the field; null if the field does not exist</returns>
        CASABLANCA_DEPRECATED("This API is deprecated and will be removed in a future release, use json::value::at() instead.")
        value get(const utility::string_t &key) const;

        /// <summary>
        /// Erases an element of a JSON array. Throws if index is out of bounds.
        /// </summary>
        /// <param name="index">The index of the element to erase in the JSON array.</param>
        _ASYNCRTIMP void erase(size_t index);

        /// <summary>
        /// Erases an element of a JSON object. Throws if the key doesn't exist.
        /// </summary>
        /// <param name="key">The key of the element to erase in the JSON object.</param>
        _ASYNCRTIMP void erase(const utf8string& key);

        /// <summary>
        /// Erases an element of a JSON object. Throws if the key doesn't exist.
        /// </summary>
        /// <param name="key">The key of the element to erase in the JSON object.</param>
        inline void erase(const utf16string& key) {
            erase(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses an element of a JSON array. Throws when index out of bounds.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array.</param>
        /// <returns>A reference to the value.</returns>
        _ASYNCRTIMP json::value& at(size_t index);

        /// <summary>
        /// Accesses an element of a JSON array. Throws when index out of bounds.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array.</param>
        /// <returns>A reference to the value.</returns>
        _ASYNCRTIMP const json::value& at(size_t index) const;

        /// <summary>
        /// Accesses an element of a JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value.</returns>
        _ASYNCRTIMP json::value& at(const utf8string& key);

        /// <summary>
        /// Accesses an element of a JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value.</returns>
        _ASYNCRTIMP const json::value& at(const utf8string& key) const;

        /// <summary>
        /// Accesses an element of a JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value.</returns>
        inline json::value& at(const utf16string& key) {
            return at(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses an element of a JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value.</returns>
        inline const json::value& at(const utf16string& key) const {
            return at(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses a field of a JSON object.
        /// </summary>
        /// <param name="key">The name of the field</param>
        /// <returns>A reference to the value kept in the field.</returns>
        _ASYNCRTIMP value & operator [] (const utf8string &key);

        /// <summary>
        /// Accesses a field of a JSON object.
        /// </summary>
        /// <param name="key">The name of the field</param>
        /// <returns>A reference to the value kept in the field.</returns>
        inline value & operator [] (const utf16string &key) {
            return operator[](utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses an element of a JSON array.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array</param>
        /// <returns>The value kept at the array index; null if outside the boundaries of the array</returns>
        CASABLANCA_DEPRECATED("This API is deprecated and will be removed in a future release, use json::value::at() instead.")
        value get(size_t index) const;

        /// <summary>
        /// Accesses an element of a JSON array.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array.</param>
        /// <returns>A reference to the value kept in the field.</returns>
        _ASYNCRTIMP value & operator [] (size_t index);

    private:
        friend class web::json::details::_Object;
        friend class web::json::details::_Array;
        friend class web::json::details::JSON_Parser;

        /// <summary>
        /// Serializes the content of the value into a string instance in UTF8 format
        /// </summary>
        /// <param name="string">The string that the JSON representation should be written to</param>
        _ASYNCRTIMP void format(std::basic_string<char>& string) const;

        explicit value(std::unique_ptr<details::_Value> v);

        std::unique_ptr<details::_Value> m_value;
#ifdef ENABLE_JSON_VALUE_VISUALIZER
        value_type m_kind;
#endif
    };

    /// <summary>
    /// A single exception type to represent errors in parsing, converting, and accessing
    /// elements of JSON values.
    /// </summary>
    class json_exception : public std::exception
    {
    private:
        std::string _message;
    public:
        CASABLANCA_DEPRECATED("This API is deprecated and will be removed in a future release, use the UTF-8 constructor instead.")
        json_exception(const utility::char_t * const &message) : _message(utility::conversions::to_utf8string(message)) { }

        json_exception(utf8string message) : _message(std::move(message)) { }

        // Must be narrow string because it derives from std::exception
        const char* what() const CPPREST_NOEXCEPT
        {
            return _message.c_str();
        }
    };

    namespace details
    {
        enum json_error
        {
            left_over_character_in_stream = 1,
            malformed_array_literal,
            malformed_comment,
            malformed_literal,
            malformed_object_literal,
            malformed_numeric_literal,
            malformed_string_literal,
            malformed_token,
            mismatched_brances,
            nesting,
            unexpected_token
        };

        class json_error_category_impl : public std::error_category
        {
        public:
            virtual const char* name() const CPPREST_NOEXCEPT override
            {
                return "json";
            }

            virtual std::string message(int ev) const override
            {
                switch (ev)
                {
                case json_error::left_over_character_in_stream:
                    return "Left-over characters in stream after parsing a JSON value";
                case json_error::malformed_array_literal:
                    return "Malformed array literal";
                case json_error::malformed_comment:
                    return "Malformed comment";
                case json_error::malformed_literal:
                    return "Malformed literal";
                case json_error::malformed_object_literal:
                    return "Malformed object literal";
                case json_error::malformed_numeric_literal:
                    return "Malformed numeric literal";
                case json_error::malformed_string_literal:
                    return "Malformed string literal";
                case json_error::malformed_token:
                    return "Malformed token";
                case json_error::mismatched_brances:
                    return "Mismatched braces";
                case json_error::nesting:
                    return "Nesting too deep";
                case json_error::unexpected_token:
                    return "Unexpected token";
                default:
                    return "Unknown json error";
                }
            }
        };

        const json_error_category_impl& json_error_category();
    }

    /// <summary>
    /// A JSON array represented as a C++ class.
    /// </summary>
    class array
    {
        typedef std::vector<json::value> storage_type;

    public:
        typedef storage_type::iterator iterator;
        typedef storage_type::const_iterator const_iterator;
        typedef storage_type::reverse_iterator reverse_iterator;
        typedef storage_type::const_reverse_iterator const_reverse_iterator;
        typedef storage_type::size_type size_type;

    private:
        array() : m_elements() { }
        array(size_type size) : m_elements(size) { }
        array(storage_type elements) : m_elements(std::move(elements)) { }

    public:
        /// <summary>
        /// Gets the beginning iterator element of the array
        /// </summary>
        /// <returns>An <c>iterator</c> to the beginning of the JSON array.</returns>
        iterator begin()
        {
            return m_elements.begin();
        }

        /// <summary>
        /// Gets the beginning const iterator element of the array.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the beginning of the JSON array.</returns>
        const_iterator begin() const
        {
            return m_elements.cbegin();
        }

        /// <summary>
        /// Gets the end iterator element of the array
        /// </summary>
        /// <returns>An <c>iterator</c> to the end of the JSON array.</returns>
        iterator end()
        {
            return m_elements.end();
        }

        /// <summary>
        /// Gets the end const iterator element of the array.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the end of the JSON array.</returns>
        const_iterator end() const
        {
            return m_elements.cend();
        }

        /// <summary>
        /// Gets the beginning reverse iterator element of the array
        /// </summary>
        /// <returns>An <c>reverse_iterator</c> to the beginning of the JSON array.</returns>
        reverse_iterator rbegin()
        {
            return m_elements.rbegin();
        }

        /// <summary>
        /// Gets the beginning const reverse iterator element of the array
        /// </summary>
        /// <returns>An <c>const_reverse_iterator</c> to the beginning of the JSON array.</returns>
        const_reverse_iterator rbegin() const
        {
            return m_elements.rbegin();
        }

        /// <summary>
        /// Gets the end reverse iterator element of the array
        /// </summary>
        /// <returns>An <c>reverse_iterator</c> to the end of the JSON array.</returns>
        reverse_iterator rend()
        {
            return m_elements.rend();
        }

        /// <summary>
        /// Gets the end const reverse iterator element of the array
        /// </summary>
        /// <returns>An <c>const_reverse_iterator</c> to the end of the JSON array.</returns>
        const_reverse_iterator rend() const
        {
            return m_elements.crend();
        }

        /// <summary>
        /// Gets the beginning const iterator element of the array.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the beginning of the JSON array.</returns>
        const_iterator cbegin() const
        {
            return m_elements.cbegin();
        }

        /// <summary>
        /// Gets the end const iterator element of the array.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the end of the JSON array.</returns>
        const_iterator cend() const
        {
            return m_elements.cend();
        }

        /// <summary>
        /// Gets the beginning const reverse iterator element of the array.
        /// </summary>
        /// <returns>A <c>const_reverse_iterator</c> to the beginning of the JSON array.</returns>
        const_reverse_iterator crbegin() const
        {
            return m_elements.crbegin();
        }

        /// <summary>
        /// Gets the end const reverse iterator element of the array.
        /// </summary>
        /// <returns>A <c>const_reverse_iterator</c> to the end of the JSON array.</returns>
        const_reverse_iterator crend() const
        {
            return m_elements.crend();
        }

        /// <summary>
        /// Deletes an element of the JSON array.
        /// </summary>
        /// <param name="position">A const_iterator to the element to delete.</param>
        /// <returns>Iterator to the new location of the element following the erased element.</returns>
        /// <remarks>GCC doesn't support erase with const_iterator on vector yet. In the future this should be changed.</remarks>
        iterator erase(iterator position)
        {
            return m_elements.erase(position);
        }

        /// <summary>
        /// Deletes the element at an index of the JSON array.
        /// </summary>
        /// <param name="index">The index of the element to delete.</param>
        void erase(size_type index)
        {
            if (index >= m_elements.size())
            {
                throw json_exception("index out of bounds");
            }
            m_elements.erase(m_elements.begin() + index);
        }

        /// <summary>
        /// Accesses an element of a JSON array. Throws when index out of bounds.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array.</param>
        /// <returns>A reference to the value kept in the field.</returns>
        json::value& at(size_type index)
        {
            if (index >= m_elements.size())
                throw json_exception("index out of bounds");

            return m_elements[index];
        }

        /// <summary>
        /// Accesses an element of a JSON array. Throws when index out of bounds.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array.</param>
        /// <returns>A reference to the value kept in the field.</returns>
        const json::value& at(size_type index) const
        {
            if (index >= m_elements.size())
                throw json_exception("index out of bounds");

            return m_elements[index];
        }

        /// <summary>
        /// Accesses an element of a JSON array.
        /// </summary>
        /// <param name="index">The index of an element in the JSON array.</param>
        /// <returns>A reference to the value kept in the field.</returns>
        json::value& operator[](size_type index)
        {
            msl::safeint3::SafeInt<size_type> nMinSize(index);
            nMinSize += 1;
            msl::safeint3::SafeInt<size_type> nlastSize(m_elements.size());
            if (nlastSize < nMinSize)
                m_elements.resize(nMinSize);

            return m_elements[index];
        }

        /// <summary>
        /// Gets the number of elements of the array.
        /// </summary>
        /// <returns>The number of elements.</returns>
        size_type size() const
        {
            return m_elements.size();
        }

    private:
        storage_type m_elements;

        friend class details::_Array;
        friend class json::details::JSON_Parser;
    };

    /// <summary>
    /// A JSON object represented as a C++ class.
    /// </summary>
    class object
    {
        typedef std::vector<std::pair<utf8string, json::value>> storage_type;

    public:
        typedef storage_type::iterator iterator;
        typedef storage_type::const_iterator const_iterator;
        typedef storage_type::reverse_iterator reverse_iterator;
        typedef storage_type::const_reverse_iterator const_reverse_iterator;
        typedef storage_type::size_type size_type;

    private:
        object(bool keep_order = false) : m_elements(), m_keep_order(keep_order) { }
        object(storage_type elements, bool keep_order = false) : m_elements(std::move(elements)), m_keep_order(keep_order)
        {
            if (!keep_order) {
                sort(m_elements.begin(), m_elements.end(), compare_pairs);
            }
        }

    public:
        /// <summary>
        /// Gets the beginning iterator element of the object
        /// </summary>
        /// <returns>An <c>iterator</c> to the beginning of the JSON object.</returns>
        iterator begin()
        {
            return m_elements.begin();
        }

        /// <summary>
        /// Gets the beginning const iterator element of the object.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the beginning of the JSON object.</returns>
        const_iterator begin() const
        {
            return m_elements.cbegin();
        }

        /// <summary>
        /// Gets the end iterator element of the object
        /// </summary>
        /// <returns>An <c>iterator</c> to the end of the JSON object.</returns>
        iterator end()
        {
            return m_elements.end();
        }

        /// <summary>
        /// Gets the end const iterator element of the object.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the end of the JSON object.</returns>
        const_iterator end() const
        {
            return m_elements.cend();
        }

        /// <summary>
        /// Gets the beginning reverse iterator element of the object
        /// </summary>
        /// <returns>An <c>reverse_iterator</c> to the beginning of the JSON object.</returns>
        reverse_iterator rbegin()
        {
            return m_elements.rbegin();
        }

        /// <summary>
        /// Gets the beginning const reverse iterator element of the object
        /// </summary>
        /// <returns>An <c>const_reverse_iterator</c> to the beginning of the JSON object.</returns>
        const_reverse_iterator rbegin() const
        {
            return m_elements.rbegin();
        }

        /// <summary>
        /// Gets the end reverse iterator element of the object
        /// </summary>
        /// <returns>An <c>reverse_iterator</c> to the end of the JSON object.</returns>
        reverse_iterator rend()
        {
            return m_elements.rend();
        }

        /// <summary>
        /// Gets the end const reverse iterator element of the object
        /// </summary>
        /// <returns>An <c>const_reverse_iterator</c> to the end of the JSON object.</returns>
        const_reverse_iterator rend() const
        {
            return m_elements.crend();
        }

        /// <summary>
        /// Gets the beginning const iterator element of the object.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the beginning of the JSON object.</returns>
        const_iterator cbegin() const
        {
            return m_elements.cbegin();
        }

        /// <summary>
        /// Gets the end const iterator element of the object.
        /// </summary>
        /// <returns>A <c>const_iterator</c> to the end of the JSON object.</returns>
        const_iterator cend() const
        {
            return m_elements.cend();
        }

        /// <summary>
        /// Gets the beginning const reverse iterator element of the object.
        /// </summary>
        /// <returns>A <c>const_reverse_iterator</c> to the beginning of the JSON object.</returns>
        const_reverse_iterator crbegin() const
        {
            return m_elements.crbegin();
        }

        /// <summary>
        /// Gets the end const reverse iterator element of the object.
        /// </summary>
        /// <returns>A <c>const_reverse_iterator</c> to the end of the JSON object.</returns>
        const_reverse_iterator crend() const
        {
            return m_elements.crend();
        }

        /// <summary>
        /// Deletes an element of the JSON object.
        /// </summary>
        /// <param name="position">A const_iterator to the element to delete.</param>
        /// <returns>Iterator to the new location of the element following the erased element.</returns>
        /// <remarks>GCC doesn't support erase with const_iterator on vector yet. In the future this should be changed.</remarks>
        iterator erase(iterator position)
        {
            return m_elements.erase(position);
        }

        /// <summary>
        /// Deletes an element of the JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        void erase(const utf8string &key)
        {
            auto iter = find_by_key(key);
            if (iter == m_elements.end())
            {
                throw web::json::json_exception("Key not found");
            }

            m_elements.erase(iter);
        }
        void erase(const utf16string& key)
        {
            erase(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses an element of a JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value kept in the field.</returns>
        json::value& at(const utf8string& key)
        {
            auto iter = find_by_key(key);
            if (iter == m_elements.end())
            {
                throw web::json::json_exception("Key not found");
            }

            return iter->second;
        }
        json::value& at(const utf16string& key)
        {
            return at(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses an element of a JSON object. If the key doesn't exist, this method throws.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value kept in the field.</returns>
        const json::value& at(const utf8string& key) const
        {
            auto iter = find_by_key(key);
            if (iter == m_elements.end())
            {
                throw web::json::json_exception("Key not found");
            }

            return iter->second;
        }
        const json::value& at(const utf16string& key) const
        {
            return at(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Accesses an element of a JSON object.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>If the key exists, a reference to the value kept in the field, otherwise a newly created null value that will be stored for the given key.</returns>
        json::value& operator[](const utf8string& key)
        {
            auto iter = find_insert_location(key);

            if (iter == m_elements.end() || key != iter->first)
            {
                return m_elements.insert(iter, std::pair<utf8string, value>(key, value()))->second;
            }

            return iter->second;
        }
        json::value& operator[](const utf16string& key)
        {
            return operator[](utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Gets an iterator to an element of a JSON object.
        /// </summary>
        /// <param name="key">The key of an element in the JSON object.</param>
        /// <returns>A const iterator to the value kept in the field.</returns>
        const_iterator find(const utf8string& key) const
        {
            return find_by_key(key);
        }
        const_iterator find(const utf16string& key) const
        {
            return find(utility::conversions::to_utf8string(key));
        }

        /// <summary>
        /// Gets the number of elements of the object.
        /// </summary>
        /// <returns>The number of elements.</returns>
        size_type size() const
        {
            return m_elements.size();
        }

        /// <summary>
        /// Checks if there are any elements in the JSON object.
        /// </summary>
        /// <returns>True iff empty.</returns>
        bool empty() const
        {
            return m_elements.empty();
        }
    private:

        static bool compare_pairs(const std::pair<utf8string, value>& p1, const std::pair<utf8string, value>& p2)
        {
            return p1.first < p2.first;
        }
        static bool compare_with_key(const std::pair<utf8string, value>& p1, const utf8string& key)
        {
            return p1.first < key;
        }

        storage_type::iterator find_insert_location(const utf8string &key)
        {
            if (m_keep_order)
            {
                return std::find_if(m_elements.begin(), m_elements.end(),
                    [&key](const std::pair<utf8string, value>& p) {
                    return p.first == key;
                });
            }
            else
            {
                return std::lower_bound(m_elements.begin(), m_elements.end(), key, compare_with_key);
            }
        }

        storage_type::const_iterator find_by_key(const utf8string& key) const
        {
            if (m_keep_order)
            {
                return std::find_if(m_elements.begin(), m_elements.end(),
                    [&key](const std::pair<utf8string, value>& p) {
                    return p.first == key;
                });
            }
            else
            {
                auto iter = std::lower_bound(m_elements.begin(), m_elements.end(), key, compare_with_key);
                if (iter != m_elements.end() && key != iter->first)
                {
                    return m_elements.end();
                }
                return iter;
            }
        }

        storage_type::iterator find_by_key(const utf8string& key)
        {
            auto iter = find_insert_location(key);
            if (iter != m_elements.end() && key != iter->first)
            {
                return m_elements.end();
            }
            return iter;
        }

        storage_type m_elements;
        bool m_keep_order;
        friend class details::_Object;

        friend class json::details::JSON_Parser;
   };

    /// <summary>
    /// A JSON number represented as a C++ class.
    /// </summary>
    class number
    {
        // Note that these constructors make sure that only negative integers are stored as signed int64 (while others convert to unsigned int64).
        // This helps handling number objects e.g. comparing two numbers.

        number(double value)  : m_value(value), m_type(double_type) { }
        number(int32_t value) : m_intval(value), m_type(value < 0 ? signed_type : unsigned_type) { }
        number(uint32_t value) : m_intval(value), m_type(unsigned_type) { }
        number(int64_t value) : m_intval(value), m_type(value < 0 ? signed_type : unsigned_type) { }
        number(uint64_t value) : m_uintval(value), m_type(unsigned_type) { }

    public:

        /// <summary>
        /// Does the number fit into int32?
        /// </summary>
        /// <returns><c>true</c> if the number fits into int32, <c>false</c> otherwise</returns>
        _ASYNCRTIMP bool is_int32() const;

        /// <summary>
        /// Does the number fit into unsigned int32?
        /// </summary>
        /// <returns><c>true</c> if the number fits into unsigned int32, <c>false</c> otherwise</returns>
        _ASYNCRTIMP bool is_uint32() const;

        /// <summary>
        /// Does the number fit into int64?
        /// </summary>
        /// <returns><c>true</c> if the number fits into int64, <c>false</c> otherwise</returns>
        _ASYNCRTIMP bool is_int64() const;

        /// <summary>
        /// Does the number fit into unsigned int64?
        /// </summary>
        /// <returns><c>true</c> if the number fits into unsigned int64, <c>false</c> otherwise</returns>
        bool is_uint64() const
        {
            switch (m_type)
            {
            case signed_type : return m_intval >= 0;
            case unsigned_type : return true;
            case double_type :
            default :
                return false;
            }
        }

        /// <summary>
        /// Converts the JSON number to a C++ double.
        /// </summary>
        /// <returns>A double representation of the number</returns>
        double to_double() const
        {
            switch (m_type)
            {
            case double_type : return m_value;
            case signed_type : return static_cast<double>(m_intval);
            case unsigned_type : return static_cast<double>(m_uintval);
            default : return false;
            }
        }

        /// <summary>
        /// Converts the JSON number to int32.
        /// </summary>
        /// <returns>An int32 representation of the number</returns>
        int32_t to_int32() const
        {
            if (m_type == double_type)
                return static_cast<int32_t>(m_value);
            else
                return static_cast<int32_t>(m_intval);
        }

        /// <summary>
        /// Converts the JSON number to unsigned int32.
        /// </summary>
        /// <returns>An usigned int32 representation of the number</returns>
        uint32_t to_uint32() const
        {
            if (m_type == double_type)
                return static_cast<uint32_t>(m_value);
            else
                return static_cast<uint32_t>(m_intval);
        }

        /// <summary>
        /// Converts the JSON number to int64.
        /// </summary>
        /// <returns>An int64 representation of the number</returns>
        int64_t to_int64() const
        {
            if (m_type == double_type)
                return static_cast<int64_t>(m_value);
            else
                return static_cast<int64_t>(m_intval);
        }

        /// <summary>
        /// Converts the JSON number to unsigned int64.
        /// </summary>
        /// <returns>An unsigned int64 representation of the number</returns>
        uint64_t to_uint64() const
        {
            if (m_type == double_type)
                return static_cast<uint64_t>(m_value);
            else
                return static_cast<uint64_t>(m_intval);
        }

        /// <summary>
        /// Is the number represented internally as an integral type?
        /// </summary>
        /// <returns><c>true</c> if the number is represented as an integral type, <c>false</c> otherwise</returns>
        bool is_integral() const
        {
            return m_type != double_type;
        }

        /// <summary>
        /// Compares two JSON numbers for equality.
        /// </summary>
        /// <param name="other">The JSON number to compare with.</param>
        /// <returns>True iff the numbers are equal.</returns>
        bool operator==(const number &other) const
        {
            if (m_type != other.m_type)
                return false;

            switch (m_type)
            {
            case json::number::type::signed_type :
                return m_intval == other.m_intval;
            case json::number::type::unsigned_type :
                return m_uintval == other.m_uintval;
            case json::number::type::double_type :
                return m_value == other.m_value;
            }
            __assume(0);
        }

    private:
        union
        {
            int64_t m_intval;
            uint64_t m_uintval;
            double  m_value;
        };

        enum type
        {
            signed_type=0, unsigned_type, double_type
        } m_type;

        friend class details::_Number;
    };

    namespace details
    {
        /// <summary>
        /// Internal interface for virtual hierarchy of json types
        /// </summary>
        class _Value
        {
        public:
            virtual std::unique_ptr<_Value> _copy_value() const = 0;

            virtual bool has_field(const utf8string&) const { return false; }
            virtual value get_field(const utf8string&) const { throw json_exception("not an object"); }
            virtual value get_element(array::size_type) const { throw json_exception("not an array"); }

            virtual value &index(const utf8string &) { throw json_exception("not an object"); }
            virtual value &index(array::size_type) { throw json_exception("not an array"); }

            virtual const value &cnst_index(const utf8string&) const { throw json_exception("not an object"); }
            virtual const value &cnst_index(array::size_type) const { throw json_exception("not an array"); }

            /// <summary>
            /// Appends a UTF-8 serialized form of the object tree onto the passed in string.
            /// </summary>
            virtual void serialize_impl(utf8string& str) const = 0;
            /// <summary>
            /// Calculate an estimate of how many UTF-8 characters will be used during serialization.
            /// </summary>
            virtual size_t serialize_size() const = 0;

            virtual json::value::value_type type() const = 0;

            virtual bool is_integer() const { throw json_exception("not a number"); }
            virtual bool is_double() const { throw json_exception("not a number"); }

            virtual const json::number& as_number() { throw json_exception("not a number"); }
            virtual double as_double() const { throw json_exception("not a number"); }
            virtual int as_integer() const { throw json_exception("not a number"); }
            virtual bool as_bool() const { throw json_exception("not a boolean"); }
            virtual json::array& as_array() { throw json_exception("not an array"); }
            virtual const json::array& as_array() const { throw json_exception("not an array"); }
            virtual json::object& as_object() { throw json_exception("not an object"); }
            virtual const json::object& as_object() const { throw json_exception("not an object"); }
            virtual const utf8string& as_string() const { throw json_exception("not a string"); }

            virtual size_t size() const {
                return 0;
            }

            virtual ~_Value() {}

        protected:
            _Value() {}

        private:

            friend class web::json::value;
        };

        class _Null final : public _Value
        {
        public:
            virtual std::unique_ptr<_Value> _copy_value() const override
            {
                return utility::details::make_unique<_Null>();
            }
            virtual json::value::value_type type() const override { return json::value::Null; }
            virtual void serialize_impl(utf8string& str) const override {
                str.append("null");
            }
            virtual size_t serialize_size() const override { return 4; }
        };

        class _Number final : public _Value
        {
        public:
            template<class T>
            explicit _Number(T t) : m_number(t) { }

            virtual std::unique_ptr<_Value> _copy_value() const override
            {
                return utility::details::make_unique<_Number>(*this);
            }

            virtual json::value::value_type type() const override { return json::value::Number; }

            virtual void serialize_impl(utf8string& stream) const override;
            virtual size_t serialize_size() const override {
                return 10; // fast estimate
            }

            virtual bool is_integer() const override { return m_number.is_integral(); }
            virtual bool is_double() const override { return !m_number.is_integral(); }

            virtual double as_double() const override
            {
                return m_number.to_double();
            }

            virtual int as_integer() const override
            {
                return m_number.to_int32();
            }

            virtual const number& as_number() override { return m_number; }

        private:
            json::number m_number;
        };

        class _Boolean final : public _Value
        {
        public:
            _Boolean(bool value) : m_value(value) { }

            virtual std::unique_ptr<_Value> _copy_value() const override
            {
                return utility::details::make_unique<_Boolean>(*this);
            }

            virtual json::value::value_type type() const override { return json::value::Boolean; }

            virtual bool as_bool() const override { return m_value; }

            virtual void serialize_impl(utf8string& stream) const override
            {
                stream.append(m_value ? "true" : "false");
            }
            virtual size_t serialize_size() const override {
                return 5;
            }

        private:
            friend class json::details::JSON_Parser;
            bool m_value;
        };

        class _UTF8String final : public _Value
        {
        public:

            _UTF8String(utf8string value) : m_string(std::move(value))
            {
                m_has_escape_char = has_escape_chars(*this);
            }
            _UTF8String(utf8string value, bool escaped_chars)
                : m_string(std::move(value)),
                m_has_escape_char(escaped_chars)
            { }

            virtual std::unique_ptr<_Value> _copy_value() const override
            {
                return utility::details::make_unique<_UTF8String>(*this);
            }

            virtual json::value::value_type type() const override { return json::value::String; }

            virtual const utf8string & as_string() const override { return m_string; }

            virtual void serialize_impl(utf8string& str) const override;
            virtual size_t serialize_size() const override {
                return m_string.size() + 2;
            }

        private:
            friend class _Object;
            friend class _Array;

            utf8string m_string;

            // There are significant performance gains that can be made by knowning whether
            // or not a character that requires escaping is present.
            bool m_has_escape_char;
            static bool has_escape_chars(const _UTF8String &str);
        };

        template<typename CharType>
        _ASYNCRTIMP void append_escape_string(std::basic_string<CharType>& str, const std::basic_string<CharType>& escaped);

        void format_string(const utf8string& key, utf8string& str);

        class _Object final : public _Value
        {
        public:

            _Object(object::storage_type fields, bool keep_order) : m_object(std::move(fields), keep_order) { }

            virtual std::unique_ptr<_Value> _copy_value() const override
            {
                return utility::details::make_unique<_Object>(*this);
            }

            virtual json::object& as_object() override { return m_object; }

            virtual const json::object& as_object() const override { return m_object; }

            virtual json::value::value_type type() const override { return json::value::Object; }

            virtual bool has_field(const utf8string& key) const override;

            virtual json::value &index(const utf8string& key) override;

            bool is_equal(const _Object* other) const
            {
                if (m_object.size() != other->m_object.size())
                    return false;
                return std::equal(std::begin(m_object), std::end(m_object), std::begin(other->m_object));
            }

            virtual void serialize_impl(utf8string& str) const override
            {
                str.push_back('{');
                if (!m_object.empty())
                {
                    auto lastElement = m_object.end() - 1;
                    for (auto iter = m_object.begin(); iter != lastElement; ++iter)
                    {
                        format_string(iter->first, str);
                        str.push_back(':');
                        iter->second.format(str);
                        str.push_back(',');
                    }
                    format_string(lastElement->first, str);
                    str.push_back(':');
                    lastElement->second.format(str);
                }
                str.push_back('}');
            }

            virtual size_t serialize_size() const override
            {
                // This is a heuristic we can tune more in the future:
                // Basically size of string plus
                // sum size of value if an object, array, or string.
                size_t reserveSize = 2; // For brackets {}
                for (auto iter = m_object.begin(); iter != m_object.end(); ++iter)
                {
                    reserveSize += iter->first.length() + 2;    // 2 for quotes
                    size_t valueSize = iter->second.size() * 20; // Multipler by each object/array element
                    if (valueSize == 0)
                    {
                        valueSize = iter->second.m_value->serialize_size();
                    }
                    reserveSize += valueSize;
                }
                return reserveSize;
            }

            virtual size_t size() const override { return m_object.size(); }

        private:
            json::object m_object;

            friend class json::details::JSON_Parser;

        };

        class _Array final : public _Value
        {
        public:
            _Array(array::size_type size = 0) : m_array(size) {}
            _Array(array::storage_type elements) : m_array(std::move(elements)) { }

            virtual std::unique_ptr<_Value> _copy_value() const override
            {
                return utility::details::make_unique<_Array>(*this);
            }

            virtual json::value::value_type type() const override { return json::value::Array; }

            virtual json::array& as_array() override { return m_array; }
            virtual const json::array& as_array() const override { return m_array; }

            virtual json::value &index(json::array::size_type index) override
            {
                return m_array[index];
            }

            bool is_equal(const _Array* other) const
            {
                if ( m_array.size() != other->m_array.size())
                    return false;

                auto iterT  = m_array.cbegin();
                auto iterO  = other->m_array.cbegin();
                auto iterTe = m_array.cend();
                auto iterOe = other->m_array.cend();

                for (; iterT != iterTe && iterO != iterOe; ++iterT, ++iterO)
                {
                    if ( *iterT != *iterO )
                        return false;
                }

                return true;
            }

            virtual void serialize_impl(std::string& str) const override
            {
                str.push_back('[');
                if (!m_array.m_elements.empty())
                {
                    auto lastElement = m_array.m_elements.end() - 1;
                    for (auto iter = m_array.m_elements.begin(); iter != lastElement; ++iter)
                    {
                        iter->format(str);
                        str.push_back(',');
                    }
                    lastElement->format(str);
                }
                str.push_back(']');
            }
            virtual size_t serialize_size() const override {
                // This is a heuristic we can tune more in the future:
                // Basically sum size of each value if an object, array, or string by a multiplier.
                size_t reserveSize = 2; // For brackets []
                for (auto iter = m_array.cbegin(); iter != m_array.cend(); ++iter)
                {
                    size_t valueSize = iter->size() * 20; // Per each nested array/object

                    if (valueSize == 0)
                        valueSize = 5; // true, false, or null

                    reserveSize += valueSize;
                }
                return reserveSize;
            }
            virtual size_t size() const override { return m_array.size(); }

        private:
            json::array m_array;

            friend class json::details::JSON_Parser;
        };
    } // namespace details

    /// <summary>
    /// Gets the number of children of the value.
    /// </summary>
    /// <returns>The number of children. 0 for all non-composites.</returns>
    inline size_t json::value::size() const
    {
        return m_value->size();
    }

    /// <summary>
    /// Test for the presence of a field.
    /// </summary>
    /// <param name="key">The name of the field</param>
    /// <returns>True if the field exists, false otherwise.</returns>
    inline bool json::value::has_field(const utf8string& key) const
    {
        return m_value->has_field(key);
    }

    /// <summary>
    /// Access a field of a JSON object.
    /// </summary>
    /// <param name="key">The name of the field</param>
    /// <returns>The value kept in the field; null if the field does not exist</returns>
    inline json::value json::value::get(const utility::string_t& key) const
    {
        return m_value->get_field(utility::conversions::to_utf8string(key));
    }

    inline json::value::value(std::unique_ptr<web::json::details::_Value> v)
        : m_value(std::move(v))
#ifdef ENABLE_JSON_VALUE_VISUALIZER
        , m_kind(m_value->type())
#endif
    {}

    /// <summary>
    /// Access an element of a JSON array.
    /// </summary>
    /// <param name="index">The index of an element in the JSON array</param>
    /// <returns>The value kept at the array index; null if outside the boundaries of the array</returns>
    inline json::value json::value::get(size_t index) const
    {
        return m_value->get_element(index);
    }

    /// <summary>
    /// A standard <c>std::ostream</c> operator to facilitate writing JSON values to streams.
    /// </summary>
    /// <param name="os">The output stream to write the JSON value to.</param>
    /// <param name="val">The JSON value to be written to the stream.</param>
    /// <returns>The output stream object</returns>
    _ASYNCRTIMP utility::ostream_t& __cdecl operator << (utility::ostream_t &os, const json::value &val);

    /// <summary>
    /// A standard <c>std::istream</c> operator to facilitate reading JSON values from streams.
    /// </summary>
    /// <param name="is">The input stream to read the JSON value from.</param>
    /// <param name="val">The JSON value object read from the stream.</param>
    /// <returns>The input stream object.</returns>
    _ASYNCRTIMP utility::istream_t& __cdecl operator >> (utility::istream_t &is, json::value &val);
}}

#endif

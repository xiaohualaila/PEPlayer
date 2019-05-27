#include "XmlRpcTX2Value.h"
#include "base64.h"
#include <iostream>
#include <exception>

namespace XmlRpcTX2Value {
    static const char VALUE_TAG[] = "value";
    static const char BOOLEAN_TAG[] = "boolean";
    static const char DOUBLE_TAG[] = "double";
    static const char INT_TAG[] = "int";
    static const char I4_TAG[] = "i4";
    static const char STRING_TAG[] = "string";
    static const char DATETIME_TAG[] = "dateTime.iso8601";
    static const char BASE64_TAG[] = "base64";
    static const char ARRAY_TAG[] = "array";
    static const char DATA_TAG[] = "data";
    static const char STRUCT_TAG[] = "struct";
    static const char MEMBER_TAG[] = "member";
    static const char NAME_TAG[] = "name";

	// Format strings
	std::string XmlRpcValue::_doubleFormat("%f");

	// Clean up
	void XmlRpcValue::invalidate()
	{
		switch (_type) {
		case TypeString:    delete _value.asString; break;
		case TypeDateTime:  delete _value.asTime;   break;
		case TypeBase64:    delete _value.asBinary; break;
		case TypeArray:     delete _value.asArray;  break;
		case TypeStruct:    delete _value.asStruct; break;
		default: break;
		}
		_type = TypeInvalid;
		_value.asBinary = 0;
	}


	// Type checking
	void XmlRpcValue::assertTypeOrInvalid(Type t)
	{
		if (_type == TypeInvalid)
		{
			_type = t;
			switch (_type) {    // Ensure there is a valid value for the type
			case TypeString:   _value.asString = new std::string(); break;
			case TypeDateTime: _value.asTime = new struct tm();     break;
			case TypeBase64:   _value.asBinary = new BinaryData();  break;
			case TypeArray:    _value.asArray = new ValueArray();   break;
			case TypeStruct:   _value.asStruct = new ValueStruct(); break;
			default:           _value.asBinary = 0; break;
			}
		}
		else if (_type != t)
			throw std::exception();//"type error"
	}

	void XmlRpcValue::assertArray(int size) const
	{
		if (_type != TypeArray)
            throw std::exception();//"type error: expected an array"
		else if (int(_value.asArray->size()) < size)
            throw std::exception();//"range error: array index too large"
	}


	void XmlRpcValue::assertArray(int size)
	{
		if (_type == TypeInvalid) {
			_type = TypeArray;
			_value.asArray = new ValueArray(size);
		}
		else if (_type == TypeArray) {
			if (int(_value.asArray->size()) < size)
				_value.asArray->resize(size);
		}
		else
            throw std::exception();//"type error: expected an array"
	}

	void XmlRpcValue::assertStruct()
	{
		if (_type == TypeInvalid) {
			_type = TypeStruct;
			_value.asStruct = new ValueStruct();
		}
		else if (_type != TypeStruct)
            throw std::exception();//"type error: expected a struct"
	}


	// Operators
	XmlRpcValue& XmlRpcValue::operator=(XmlRpcValue const& rhs)
	{
		if (this != &rhs)
		{
			invalidate();
			_type = rhs._type;
			switch (_type) {
			case TypeBoolean:  _value.asBool = rhs._value.asBool; break;
			case TypeInt:      _value.asInt = rhs._value.asInt; break;
			case TypeDouble:   _value.asDouble = rhs._value.asDouble; break;
			case TypeDateTime: _value.asTime = new struct tm(*rhs._value.asTime); break;
			case TypeString:   _value.asString = new std::string(*rhs._value.asString); break;
			case TypeBase64:   _value.asBinary = new BinaryData(*rhs._value.asBinary); break;
			case TypeArray:    _value.asArray = new ValueArray(*rhs._value.asArray); break;
			case TypeStruct:   _value.asStruct = new ValueStruct(*rhs._value.asStruct); break;
			default:           _value.asBinary = 0; break;
			}
		}
		return *this;
	}


	// Predicate for tm equality
	static bool tmEq(struct tm const& t1, struct tm const& t2) {
		return t1.tm_sec == t2.tm_sec && t1.tm_min == t2.tm_min &&
			t1.tm_hour == t2.tm_hour && t1.tm_mday == t1.tm_mday &&
			t1.tm_mon == t2.tm_mon && t1.tm_year == t2.tm_year;
	}

	bool XmlRpcValue::operator==(XmlRpcValue const& other) const
	{
		if (_type != other._type)
			return false;

		switch (_type) {
		case TypeBoolean:  return (!_value.asBool && !other._value.asBool) ||
			(_value.asBool && other._value.asBool);
		case TypeInt:      return _value.asInt == other._value.asInt;
		case TypeDouble:   return _value.asDouble == other._value.asDouble;
		case TypeDateTime: return tmEq(*_value.asTime, *other._value.asTime);
		case TypeString:   return *_value.asString == *other._value.asString;
		case TypeBase64:   return *_value.asBinary == *other._value.asBinary;
		case TypeArray:    return *_value.asArray == *other._value.asArray;

			// The map<>::operator== requires the definition of value< for kcc
		case TypeStruct:   //return *_value.asStruct == *other._value.asStruct;
		{
							   if (_value.asStruct->size() != other._value.asStruct->size())
								   return false;

							   ValueStruct::const_iterator it1 = _value.asStruct->begin();
							   ValueStruct::const_iterator it2 = other._value.asStruct->begin();
							   while (it1 != _value.asStruct->end()) {
								   const XmlRpcValue& v1 = it1->second;
								   const XmlRpcValue& v2 = it2->second;
								   if (!(v1 == v2))
									   return false;
								   it1++;
								   it2++;
							   }
							   return true;
		}
		default: break;
		}
		return true;    // Both invalid values ...
	}

	bool XmlRpcValue::operator!=(XmlRpcValue const& other) const
	{
		return !(*this == other);
	}


	// Works for strings, binary data, arrays, and structs.
	int XmlRpcValue::size() const
	{
		switch (_type) {
		case TypeString: return int(_value.asString->size());
		case TypeBase64: return int(_value.asBinary->size());
		case TypeArray:  return int(_value.asArray->size());
		case TypeStruct: return int(_value.asStruct->size());
		default: break;
		}

        throw std::exception();//"type error"
	}

	// Checks for existence of struct member
	bool XmlRpcValue::hasMember(const std::string& name) const
	{
		return _type == TypeStruct && _value.asStruct->find(name) != _value.asStruct->end();
	}

	// Set the value from xml. The chars at *offset into valueXml 
	// should be the start of a <value> tag. Destroys any existing value.
    bool XmlRpcValue::fromXml(tinyxml2::XMLElement * value){
        invalidate();
        try{
            typeid(*value);
            if (std::string(value->Value()) != VALUE_TAG)
                return false;

            tinyxml2::XMLElement * type = value->FirstChildElement();
            std::string typeTag = type?type->Value():"";
            bool result = false;
            if (typeTag == BOOLEAN_TAG)
                result = boolFromXml(type);
            else if (typeTag == I4_TAG || typeTag == INT_TAG)
                result = intFromXml(type);
            else if (typeTag == DOUBLE_TAG)
                result = doubleFromXml(type);
            else if (typeTag.empty() || typeTag == STRING_TAG)
                result = stringFromXml(type);
            else if (typeTag == DATETIME_TAG)
                result = timeFromXml(type);
            else if (typeTag == BASE64_TAG)
                result = binaryFromXml(type);
            else if (typeTag == ARRAY_TAG)
                result = arrayFromXml(type);
            else if (typeTag == STRUCT_TAG)
                result = structFromXml(type);
            // Watch for empty/blank strings with no <string>tag
            else
            {
                result = stringFromXml(value);
            }

            return result;
        }
        catch (std::bad_typeid){
            return false;
        }
        //return true;
    }

	// Encode the Value in xml
    bool XmlRpcValue::toXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        switch (_type) {
        case TypeBoolean:  return boolToXml(doc, value);
        case TypeInt:      return intToXml(doc, value);
        case TypeDouble:   return doubleToXml(doc, value);
        case TypeString:   return stringToXml(doc, value);
        case TypeDateTime: return timeToXml(doc, value);
        case TypeBase64:   return binaryToXml(doc, value);
        case TypeArray:    return arrayToXml(doc, value);
        case TypeStruct:   return structToXml(doc, value);
        default: break;
        }
        return false;   // Invalid value
    }

	// Boolean
    bool XmlRpcValue::boolFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChild());
            std::string ValueStr = value_type->FirstChild()->Value();
            _type = TypeBoolean;
            _value.asBool = (stoi(ValueStr) == 1);
        }
        catch (std::bad_typeid){
            return false;
        }
        return true;
    }

    bool XmlRpcValue::boolToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * value_type = doc->NewElement(BOOLEAN_TAG);
        value_type->LinkEndChild(doc->NewText(_value.asBool ? "1" : "0"));
        (*value)->LinkEndChild(value_type);
        return true;
    }

	// Int
    bool XmlRpcValue::intFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChild());
            std::string ValueStr = value_type->FirstChild()->Value();
            _type = TypeInt;
            _value.asInt = stoi(ValueStr);
        }
        catch (std::bad_typeid){
            return false;
        }
        return true;
    }

    bool XmlRpcValue::intToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * value_type = doc->NewElement(I4_TAG);
        value_type->LinkEndChild(doc->NewText(std::to_string(_value.asInt).c_str()));
        (*value)->LinkEndChild(value_type);
        return true;
    }

	// Double
    bool XmlRpcValue::doubleFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChild());
            std::string ValueStr = value_type->FirstChild()->Value();
            _type = TypeDouble;
            _value.asDouble = stod(ValueStr);
        }
        catch (std::bad_typeid){
            return false;
        }
        return true;
    }

    bool XmlRpcValue::doubleToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * value_type = doc->NewElement(DOUBLE_TAG);
        value_type->LinkEndChild(doc->NewText(std::to_string(_value.asDouble).c_str()));
        (*value)->LinkEndChild(value_type);
        return true;
    }

	// String
    bool XmlRpcValue::stringFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChild());
            std::string ValueStr = value_type->FirstChild()->Value();
            _type = TypeString;
            _value.asString = new std::string(ValueStr);
        }
        catch (std::bad_typeid){
            _type = TypeString;
            _value.asString = new std::string("");
            return true;
        }
        return true;
    }

    bool XmlRpcValue::stringToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * value_type = doc->NewElement(STRING_TAG);
        value_type->LinkEndChild(doc->NewText(_value.asString->c_str()));
        (*value)->LinkEndChild(value_type);
        return true;
    }

	// DateTime (stored as a struct tm)
    bool XmlRpcValue::timeFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChild());
            std::string ValueStr = value_type->FirstChild()->Value();
            struct tm t;
            if (sscanf(ValueStr.c_str(), "%4d%2d%2dT%2d:%2d:%2d", &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &t.tm_sec) != 6)
                return false;

            t.tm_isdst = -1;
            _type = TypeDateTime;
            _value.asTime = new struct tm(t);
        }
        catch (std::bad_typeid){
            return false;
        }
        return true;
    }

    inline std::string to_string(struct tm *t){
        char buf[20];
        sprintf(buf, "%4d%02d%02dT%02d:%02d:%02d",
            t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
        buf[sizeof(buf)-1] = 0;
        return std::string(buf);
    }

    bool XmlRpcValue::timeToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * value_type = doc->NewElement(DATETIME_TAG);
        value_type->LinkEndChild(doc->NewText(to_string(_value.asTime).c_str()));
        (*value)->LinkEndChild(value_type);
        return true;
    }

	// Base64
    bool XmlRpcValue::binaryFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChild());
            std::string ValueStr = value_type->FirstChild()->Value();
            
            _type = TypeBase64;
            std::string asString = ValueStr;
            _value.asBinary = new BinaryData();
            // check whether base64 encodings can contain chars xml encodes...

            // convert from base64 to binary
            int iostatus = 0;
            base64<char> decoder;
            std::back_insert_iterator<BinaryData> ins = std::back_inserter(*(_value.asBinary));
            decoder.get(asString.begin(), asString.end(), ins, iostatus);
        }
        catch (std::bad_typeid){
            return false;
        }
        return true;
    }

    bool XmlRpcValue::binaryToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        // convert to base64
        std::vector<char> base64data;
        int iostatus = 0;
        base64<char> encoder;
        std::back_insert_iterator<std::vector<char> > ins = std::back_inserter(base64data);
        encoder.put(_value.asBinary->begin(), _value.asBinary->end(), ins, iostatus, base64<>::crlf());

        // Wrap with xml
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * value_type = doc->NewElement(BASE64_TAG);
        value_type->LinkEndChild(doc->NewText(base64data.data()));
        (*value)->LinkEndChild(value_type);
        return true;
    }

	// Array
    bool XmlRpcValue::arrayFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            typeid(*value_type);
            typeid(*value_type->FirstChildElement(DATA_TAG));
            tinyxml2::XMLElement * data = value_type->FirstChildElement(DATA_TAG);

            _type = TypeArray;
            _value.asArray = new ValueArray;
            XmlRpcValue v;

            tinyxml2::XMLElement * value = data->FirstChildElement(VALUE_TAG);
            while (value){
                if (v.fromXml(value)){
                    _value.asArray->push_back(v);       // copy...
                }else
                    break;
                value = value->NextSiblingElement(VALUE_TAG);
            }
        }
        catch (std::bad_typeid){
            return false;
        }
        return true;
    }

	// In general, its preferable to generate the xml of each element of the
	// array as it is needed rather than glomming up one big string.
    bool XmlRpcValue::arrayToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * array = doc->NewElement(ARRAY_TAG);
        (*value)->LinkEndChild(array);
        tinyxml2::XMLElement * data = doc->NewElement(DATA_TAG);
        array->LinkEndChild(data);
        
        int s = int(_value.asArray->size());
        for (int i = 0; i < s; ++i){
            tinyxml2::XMLElement * value0;
            if (_value.asArray->at(i).toXml(doc, &value0)){
                data->LinkEndChild(value0);
            }
        }
        return true;
    }

    bool XmlRpcValue::structFromXml(tinyxml2::XMLElement * value_type)
    {
        try{
            _type = TypeStruct;
            _value.asStruct = new ValueStruct;

            typeid(*value_type);
            tinyxml2::XMLElement * member = value_type->FirstChildElement(MEMBER_TAG);

            while (member){
                tinyxml2::XMLElement * name = member->FirstChildElement(NAME_TAG);
                tinyxml2::XMLElement * value = name->NextSiblingElement(VALUE_TAG);

                typeid(*name);
                typeid(*name->FirstChild());
                std::string asString = name->FirstChild()->Value();

                XmlRpcValue val(value);
                if (!val.valid()) {
                    invalidate();
                    return false;
                }

                const std::pair<const std::string, XmlRpcValue> p(asString, val);
                _value.asStruct->insert(p);

                member = member->NextSiblingElement(MEMBER_TAG);
            }
        }
        catch (std::bad_typeid){
            invalidate();
            return true;
        }
        return true;
    }


	// In general, its preferable to generate the xml of each element
	// as it is needed rather than glomming up one big string.
    bool XmlRpcValue::structToXml(tinyxml2::XMLDocument *doc, tinyxml2::XMLElement ** value){
        *value = doc->NewElement(VALUE_TAG);
        tinyxml2::XMLElement * struct_tag = doc->NewElement(STRUCT_TAG);
        (*value)->LinkEndChild(struct_tag);
        
        ValueStruct::iterator it;
        for (it = _value.asStruct->begin(); it != _value.asStruct->end(); ++it) {
            tinyxml2::XMLElement * member = doc->NewElement(MEMBER_TAG);
            tinyxml2::XMLElement * name = doc->NewElement(NAME_TAG);
            name->LinkEndChild(doc->NewText(it->first.c_str()));
            tinyxml2::XMLElement * value0;
            
            if (it->second.toXml(doc, &value0)){
                member->LinkEndChild(name);
                member->LinkEndChild(value0);
                struct_tag->LinkEndChild(member);
            }
        }
        return true;
    }

	// Write the value without xml encoding it
	std::ostream& XmlRpcValue::write(std::ostream& os) const {
		switch (_type) {
		default:           break;
		case TypeBoolean:  os << _value.asBool; break;
		case TypeInt:      os << _value.asInt; break;
		case TypeDouble:   os << _value.asDouble; break;
		case TypeString:   os << *_value.asString; break;
		case TypeDateTime:
		{
							 struct tm* t = _value.asTime;
							 char buf[20];
							 //sprintf_s(buf, sizeof(buf)-1, "%4d%02d%02dT%02d:%02d:%02d",
							 //	 t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
                             sprintf(buf, "%4d%02d%02dT%02d:%02d:%02d",
                                 t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
							 buf[sizeof(buf)-1] = 0;
							 os << buf;
							 break;
		}
		case TypeBase64:
		{
						   int iostatus = 0;
						   std::ostreambuf_iterator<char> out(os);
						   base64<char> encoder;
						   encoder.put(_value.asBinary->begin(), _value.asBinary->end(), out, iostatus, base64<>::crlf());
						   break;
		}
		case TypeArray:
		{
						  int s = int(_value.asArray->size());
						  os << '{';
						  for (int i = 0; i<s; ++i)
						  {
							  if (i > 0) os << ',';
							  _value.asArray->at(i).write(os);
						  }
						  os << '}';
						  break;
		}
		case TypeStruct:
		{
						   os << '[';
						   ValueStruct::const_iterator it;
						   for (it = _value.asStruct->begin(); it != _value.asStruct->end(); ++it)
						   {
							   if (it != _value.asStruct->begin()) os << ',';
							   os << it->first << ':';
							   it->second.write(os);
						   }
						   os << ']';
						   break;
		}

		}

		return os;
	}

} // namespace XmlRpc

// ostream
std::ostream& operator<<(std::ostream& os, XmlRpcTX2Value::XmlRpcValue& v)
{
    // If you want to output in xml format:
    //return os << v.toXml(); 
    return v.write(os);
}

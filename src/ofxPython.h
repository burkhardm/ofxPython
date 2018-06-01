#pragma once
#include "ofTypes.h"

extern "C"{
#include <Python.h>
}

#include <map>
#include <string>
#include <vector>

class ofxPythonMappingValue;
class ofxPythonAttrValue;
class ofxPythonTupleMaker;
class ofxPythonListMaker;

class ofxPythonObject
{
public:
	ofxPythonObject method(const std::string &method_name); //call method without arguments
	ofxPythonObject operator ()(); //call objects without arguments
	ofxPythonObject operator ()(ofxPythonObject); //call objects 1 argument
	ofxPythonObject operator ()(ofxPythonObject, ofxPythonObject); //call objects 2 arguments
	ofxPythonObject operator ()(ofxPythonObject, ofxPythonObject, ofxPythonObject); //call objects 3 arguments
	ofxPythonAttrValue attr(const std::string& attribute);
	ofxPythonMappingValue operator [](const std::string& key);
	ofxPythonMappingValue operator [](const char * key);
	bool isNone() const;
	bool isBool() const;
	bool isInt() const;
	bool isFloat() const;
	bool isString() const;
	bool isList() const;
	bool isTuple() const;
	bool isDict() const;
	bool asBool( ) const;
	long int asInt() const;
	double asFloat() const;
	std::string asString() const;
	std::vector<ofxPythonObject> asVector() const;
	std::map<ofxPythonObject,ofxPythonObject> asMap() const;
	static ofxPythonObject _None();
	static ofxPythonObject fromBool(bool);
	static ofxPythonObject fromInt(long int);
	static ofxPythonObject fromFloat(double);
	static ofxPythonObject fromString(const std::string&);
	// use makeTuple like this:
	// ofxPythonObject tuple = ofxPythonObject::makeTuple() << obj1 << obj2 << obj3;
	static ofxPythonTupleMaker makeTuple();
    static ofxPythonListMaker makeList();
    static ofxPythonObject makeDict();
	operator bool() const;
	const std::string repr();
	const std::string str();
protected:
	void insert_borrowed(PyObject *);
	void insert_owned(PyObject *);
    class ofxPythonObjectManaged
    {
    public:
        ofxPythonObjectManaged(PyObject*);
        ~ofxPythonObjectManaged();
        PyObject * obj;
    };
    ofPtr<ofxPythonObjectManaged> data;
    friend class ofxPython;
    friend class ofxPythonMappingValue;
    friend class ofxPythonAttrValue;
    friend class ofxPythonListMaker;
    friend class ofxPythonTupleMaker;
    friend ofxPythonObject make_object_owned(PyObject * obj, bool);
    friend ofxPythonObject make_object_borrowed(PyObject * obj, bool);
};

class ofxPython
{
public:
	ofxPython();
	virtual ~ofxPython();
	int init(); //returns the number of ofxPython instances,
				//if it's 1 you should initialize your custom bindings if any
	void reset();
	void executeScript(const std::string& path);
	void executeString(const std::string& script);
    ofxPythonObject executeStatement(const std::string& script);
	ofxPythonObject evalString(const std::string& expression);
	static ofxPythonObject getObject(const std::string& name, const std::string& module);
	ofxPythonObject getObject(const std::string& name);
	ofxPythonObject getObjectOrNone(const std::string& name);
	void setObject(const std::string& name, ofxPythonObject o);
    ofxPythonObject getLocals();
    void setVirtualEnv(const std::string & path);
    void addPath(const std::string & path);
protected:
    ofxPythonObject locals;
	// ofxPythonObject globals;
	static unsigned int instances;
	bool initialized;
    friend class ofxPythonObject::ofxPythonObjectManaged;
};

class ofxPythonOperation
{
public:
    static PyThreadState * pstate;
    static unsigned int instances;
    ofxPythonOperation();
    ~ofxPythonOperation();
};


class ofxPythonObjectLike
{
public:
	ofxPythonObject method(const std::string &method_name); //call method without arguments
	ofxPythonObject operator ()(); //call objects without arguments
	ofxPythonObject operator ()(ofxPythonObject); //call objects 1 argument
	ofxPythonObject operator ()(ofxPythonObject, ofxPythonObject); //call objects 2 arguments
	ofxPythonObject operator ()(ofxPythonObject, ofxPythonObject, ofxPythonObject); //call objects 3 arguments
	ofxPythonAttrValue attr(const std::string& attribute);
	ofxPythonMappingValue operator [](const std::string& key);
	ofxPythonMappingValue operator [](const char * key);
	bool isNone();
	bool isBool();
	bool isInt();
	bool isFloat();
	bool isString();
	bool isList();
	bool isTuple();
	bool isDict();
	bool asBool();
	long int asInt();
	double asFloat();
	std::string asString();
	std::vector<ofxPythonObject> asVector();
	std::map<ofxPythonObject,ofxPythonObject> asMap();
	operator bool();
	const std::string repr();
	const std::string str();
	virtual operator ofxPythonObject()=0;
};

class ofxPythonMappingValue: public ofxPythonObjectLike
{
	ofxPythonObject object;
	std::string key;
public:
	ofxPythonMappingValue(ofxPythonObject o, const std::string& k);
	operator ofxPythonObject();
	ofxPythonMappingValue & operator =(ofxPythonObject);
	ofxPythonMappingValue & operator =(ofxPythonMappingValue & o);
};

class ofxPythonAttrValue: public ofxPythonObjectLike
{
	ofxPythonObject object;
	std::string attribute;
public:
	ofxPythonAttrValue(ofxPythonObject o, const std::string& attr);
	operator ofxPythonObject();
	ofxPythonAttrValue & operator =(ofxPythonObject);
	ofxPythonAttrValue & operator =(ofxPythonAttrValue & o);
};

class ofxPythonTupleMaker: public ofxPythonObjectLike
{
	std::vector<ofxPythonObject> contents;
public:
	ofxPythonTupleMaker& operator<<(ofxPythonObject);
	operator ofxPythonObject();
};

class ofxPythonListMaker: public ofxPythonObjectLike
{
    std::vector<ofxPythonObject> contents;
public:
    ofxPythonListMaker& operator<<(ofxPythonObject);
    operator ofxPythonObject();
};
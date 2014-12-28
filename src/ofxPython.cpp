#include "ofxPython.h"
#include "ofMain.h"

extern "C"{
void init_openframeworks();
}


ofxPythonObject make_object_noaddref(PyObject * obj)
{
	ofxPythonObject o;
	o.insert_noaddref(obj);
	return o;
}

ofxPythonObject make_object_addref(PyObject * obj)
{
	ofxPythonObject o;
	o.insert(obj);
	return o;
}

//helper to transform a (const) std:string into a char *
class noconststring: vector<char>
{
public:
	noconststring(const string& source)
	:vector<char>(source.c_str(), source.c_str() + source.size() + 1u)
	{}
	operator char *()
	{
		return &(*this)[0];
	}
};

ofxPython::ofxPython()
{
}

ofxPython::~ofxPython()
{
}

class ofxPythonDestructor
{
public:
	ofxPythonDestructor()
	{
		Py_Initialize();
		init_openframeworks();
		//this seems to be the easiest way to add '.' to python path
		PyRun_SimpleString( "import sys\n"
							"sys.path.append('.')\n"
							"sys.path.append('data')\n");
	}
	~ofxPythonDestructor()
	{
		Py_Finalize();
	}
};

void ofxPython::init()
{
	static ofxPythonDestructor d;
	reset();
}

void ofxPython::reset()
{
	// globals = make_object_noaddref(PyDict_New());
	locals = make_object_noaddref(PyDict_New());
	//insert builtins
	locals["__builtins__"]=make_object_addref(PyEval_GetBuiltins());
}

void ofxPython::executeScript(const string& path)
{
	executeString(ofBufferFromFile(path).getText());
}

void ofxPython::executeString(const string& script)
{
	make_object_noaddref(PyRun_String(script.c_str(),Py_file_input,locals->obj,locals->obj));
}

ofxPythonObject ofxPython::evalString(const string& expression)
{
	return make_object_noaddref(PyRun_String(expression.c_str(),Py_eval_input,locals->obj,locals->obj));
}

ofxPythonObject ofxPython::getObject(const string& name, const string& module)
{

	ofxPythonObject o;
	PyObject *pName = PyString_FromString(module.c_str());
	PyObject *pModule = PyImport_Import(pName);
	Py_DECREF(pName);
	if(pModule)
	{
		PyObject * f = PyObject_GetAttrString(pModule, name.c_str());
		o.insert_noaddref(f);
	}
	else
	{
		ofLog() << "Python module not found:(";
	}
	Py_XDECREF(pModule);
	return o;
}

ofxPythonObject ofxPython::getObject(const string& name)
{
	return locals[name];
}

void ofxPythonObject::insert_noaddref(PyObject * obj)
{
	reset(new ofxPythonObjectManaged(obj));
}

void ofxPythonObject::insert(PyObject * obj)
{
	Py_XINCREF(obj);
	reset(new ofxPythonObjectManaged(obj));
}

ofxPythonObjectManaged::ofxPythonObjectManaged(PyObject * o):obj(o)
{
}
ofxPythonObjectManaged::~ofxPythonObjectManaged()
{
	Py_XDECREF(obj);
}

ofxPythonObject ofxPythonObject::method(const string &method_name)
{
	return make_object_noaddref(
		PyObject_CallMethod(get()->obj,noconststring(method_name),NULL));
}

ofxPythonObject ofxPythonObject::operator ()()
{
	return make_object_noaddref(PyObject_CallObject(get()->obj,NULL));
}

ofxPythonMappingValue ofxPythonObject::operator [](const string& key)
{
	return ofxPythonMappingValue(*this,key);
}

ofxPythonMappingValue ofxPythonObject::operator [](const char * key)
{
	return ofxPythonMappingValue(*this,string(key));
}

ofxPythonAttrValue ofxPythonObject::attr(const string& attribute)
{
	return ofxPythonAttrValue(*this, attribute);
}

const string ofxPythonObject::repr()
{
	ofxPythonObject objectsRepresentation = make_object_noaddref(
		PyObject_Repr(get()->obj));
	return string(PyString_AsString(objectsRepresentation->obj));
}

ofxPythonObject::operator bool() const
{
	return get() && (get()->obj != Py_None);
}

ofxPythonMappingValue::ofxPythonMappingValue(ofxPythonObject o, const string& k)
:object(o), key(k){}

ofxPythonMappingValue::operator ofxPythonObject()
{
	return make_object_noaddref(
		PyMapping_GetItemString(object->obj, noconststring(key)));
}

ofxPythonMappingValue& ofxPythonMappingValue::operator =(ofxPythonObject o)
{
	PyMapping_SetItemString(object->obj, noconststring(key) , o->obj);
	return *this;
}

ofxPythonAttrValue::ofxPythonAttrValue(ofxPythonObject o, const string& attr)
:object(o),attribute(attr){}

ofxPythonAttrValue::operator ofxPythonObject()
{
	return make_object_noaddref(
		PyObject_GetAttrString(object->obj,attribute.c_str()));
}

ofxPythonAttrValue & ofxPythonAttrValue::operator =(ofxPythonObject o)
{
	PyObject_SetAttrString(object->obj, attribute.c_str(), o->obj);
	return *this;
}

ofxPythonMappingValue & ofxPythonMappingValue::operator =(ofxPythonMappingValue & o)
{
	return *this = (ofxPythonObject) o;
}

ofxPythonAttrValue & ofxPythonAttrValue::operator =(ofxPythonAttrValue & o)
{
	return *this = (ofxPythonObject) o;
}

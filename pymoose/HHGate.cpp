#ifndef _pymoose_HHGate_cpp
#define _pymoose_HHGate_cpp
#include "HHGate.h"
using namespace pymoose;
const std::string HHGate::className = "HHGate";
HHGate::HHGate(Id id):PyMooseBase(id){}
HHGate::HHGate(std::string path):PyMooseBase(className, path){}
HHGate::HHGate(std::string name, Id parentId):PyMooseBase(className, name, parentId){}
HHGate::HHGate(std::string name, PyMooseBase& parent):PyMooseBase(className, name, parent){}
HHGate::~HHGate(){}
const std::string& HHGate::getType(){ return className; }

// Manually edited part
InterpolationTable* HHGate::__get_A() const
{
    return new InterpolationTable(PyMooseBase::pathToId(this->path()+"/A"));    
}
InterpolationTable* HHGate::__get_B() const
{
    return new InterpolationTable(PyMooseBase::pathToId(this->path()+"/B"));
}

void HHGate::tabFill(int xdivs, int mode)
{
    this->__get_A()->tabFill(xdivs, mode);
    this->__get_B()->tabFill(xdivs, mode);
}
void HHGate::setupAlpha(double AA, double AB, double AC , double AD, double AF, double BA, double BB, double BC, double BD, double BF, double size, double min, double max)
{
    vector<double> params;
    params.push_back(AA);
    params.push_back(AB);
    params.push_back(AC);
    params.push_back(AD);
    params.push_back(AF);
    params.push_back(BA);
    params.push_back(BB);
    params.push_back(BC);
    params.push_back(BD);
    params.push_back(BF);
    params.push_back(size);
    params.push_back(min);
    params.push_back(max);

    getContext()->setupAlpha(id_,params);    
}

void HHGate::setupTau(double AA, double AB, double AC , double AD, double AF, double BA, double BB, double BC, double BD, double BF, double size, double min, double max)
{
    vector<double> params;
    params.push_back(AA);
    params.push_back(AB);
    params.push_back(AC);
    params.push_back(AD);
    params.push_back(AF);
    params.push_back(BA);
    params.push_back(BB);
    params.push_back(BC);
    params.push_back(BD);
    params.push_back(BF);
    params.push_back(size);
    params.push_back(min);
    params.push_back(max);    
    getContext()->setupAlpha(id_,params);    
}

void HHGate::tweakAlpha()
{    
    getContext()->tweakAlpha(id_);
    
}
void HHGate::tweakTau()
{
    getContext()->tweakTau(id_);    
}


#endif

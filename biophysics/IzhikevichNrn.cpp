// IzhikevichNrn.cpp --- 
// 
// Filename: IzhikevichNrn.cpp
// Description: Izhikevich model of neuron. See Izhikevich,EM. 2003.
//              Simple model of spiking neurons. Neural Networks,
//              IEEE Transactions on. 14(6). pp 1569–1572 for
//              details.
//
// Author: subhasis ray
// Maintainer: 
// Created: Fri Apr  3 18:00:50 2009 (+0530)
// Version: 
// Last-Updated: Tue Apr  7 16:27:57 2009 (+0530)
//           By: subhasis ray
//     Update #: 187
// URL: 
// Keywords: 
// Compatibility: 
// 
// 

// Commentary: 
// 
// 
// 
// 

// Change log:
// 
// 
// 
//
/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment,
** also known as GENESIS 3 base code.
**           copyright (C) 2003-2009 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU General Public License version 2
** See the file COPYING.LIB for the full notice.
**********************************************************************/

// Code:

#include "basecode/header.h"
#include "basecode/moose.h"
#include "IzhikevichNrn.h"

const Cinfo* initIzhikevichNrnCinfo()
{
    static Finfo* processShared[] = {
        new DestFinfo( "process", Ftype1< ProcInfo >::global(),
                  RFCAST( &IzhikevichNrn::processFunc )),
        new DestFinfo( "reinit", Ftype1< ProcInfo >::global(),
                       RFCAST( &IzhikevichNrn::reinitFunc )),
    };
    static Finfo* process = new SharedFinfo("process", processShared,
                                             sizeof(processShared) / sizeof(Finfo*),
			"This is a shared message to receive Process messages from the scheduler objects. "
			"The first entry is a MsgDest for the Process operation. It has a single argument,ProcInfo, "
			"which holds lots of information about current time, thread, dt and so on. "
			"The second entry is a MsgDest for the Reinit operation. "
			"It also uses ProcInfo.");
    static Finfo* izhikevichNrnFinfos[] = {
        
        new ValueFinfo("Vmax", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getVmax),
                       RFCAST(&IzhikevichNrn::setVmax),
                       "Maximum membrane potential. Membrane potential is reset to c whenever"
                       " it reaches Vmax. NOTE: Izhikevich model specifies the PEAK voltage,"
                       " rather than THRSHOLD voltage. The threshold depends on the previous"
                       " history."),
        new ValueFinfo("c", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getC),
                       RFCAST(&IzhikevichNrn::setC),
                       "Reset potential. Membrane potential is reset to c whenever it reaches"
                       " Vmax."),
        new ValueFinfo("d", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getD),
                       RFCAST(&IzhikevichNrn::setD),
                       "Parameter d in Izhikevich model."),
        new ValueFinfo("a", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getA),
                       RFCAST(&IzhikevichNrn::setA),
                       "Parameter a in Izhikevich model."),
        new ValueFinfo("b", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getB),
                       RFCAST(&IzhikevichNrn::setB),
                       "The membrane potential."),
        new ValueFinfo("Vm", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getVm),
                       RFCAST(&IzhikevichNrn::setVm),
                       "Membrane potential, equivalent to v in Izhikevich equation."),
        new ValueFinfo("u", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getU),
                       RFCAST(&dummyFunc),
                       "Parameter u in Izhikevich equation."),
        new ValueFinfo("Im", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getInject),
                       RFCAST(&dummyFunc),
                       "Total current going through the membrane."),
        new ValueFinfo("initVm", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getInitVm),
                       RFCAST(&IzhikevichNrn::setInitVm),
                       "Initial membrane potential."),
        new ValueFinfo("initU", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getInitU),
                       RFCAST(&IzhikevichNrn::setInitU),
                       "Initial membrane potential."),
        new ValueFinfo("alpha", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getAlpha),
                       RFCAST(&IzhikevichNrn::setAlpha),
                       "Coefficient of v^2 in Izhikevich equation. Defaults to 0.04 in"
                       " physiological unit. In SI it should be 40000.0"),
        new ValueFinfo("beta", ValueFtype1<double>::global(),
                        GFCAST(&IzhikevichNrn::getBeta),
                        RFCAST(&IzhikevichNrn::setBeta),
                       "Coefficient of v in Izhikevich model. Defaults to 5 in physiological"
                       " unit, 5000.0 for SI units."),
        new ValueFinfo("gamma", ValueFtype1<double>::global(),
                       GFCAST(&IzhikevichNrn::getGamma),
                       RFCAST(&IzhikevichNrn::setGamma),
                       "Constant term in Izhikevich model. Defaults to 140 in both"
                       " physiological and SI units."),
        
        ///////////////////////////////
        // MsgSrc definition
        ///////////////////////////////
        new SrcFinfo("VmSrc", Ftype1<double>::global()),
        new SrcFinfo("eventSrc", Ftype1<double>::global(),
                     "Sends out a trigger for an event whenever Vm >= c."),
        ///////////////////////////////
        // MsgDest definition
        ///////////////////////////////
        new DestFinfo("injectDest", Ftype1< double >::global(),
                      RFCAST( &IzhikevichNrn::setInject), 
                      "Injection current into the neuron."),
        new DestFinfo("cDest", Ftype1< double >::global(),
                      RFCAST(&IzhikevichNrn::setC),
                      "Destination message to modify parameter c at runtime."),
        new DestFinfo("dDest", Ftype1< double >::global(),
                      RFCAST(&IzhikevichNrn::setD),
                      "Destination message to modify parameter d at runtime."),
        new DestFinfo("aDest", Ftype1<double>::global(),
                      RFCAST(&IzhikevichNrn::setA),
                      "Destination message modify parameter a at runtime."),
        new DestFinfo("bDest", Ftype1<double>::global(),
                      RFCAST(&IzhikevichNrn::setB),
                      "Destination message to modify parameter b at runtime"),
        ///////////////////////////////
        // SharedFinfos
        ///////////////////////////////
        process,
    };
    
    static SchedInfo schedInfo[] = { {process, 0, 0 },};

    static string doc[] = {
        "Name", "IzhikevichNrn",
        "Author", "Subhasis Ray",
        "Description", "Izhikevich model of spiking neuron "
        "(Izhikevich,EM. 2003. Simple model of spiking neurons. Neural"
        " Networks, IEEE Transactions on 14(6). pp 1569–1572).\n"
        " This class obeys the equations (in physiological units):\n"
        "  dVm/dt = 0.04 * Vm^2 + 5 * Vm + 140 - u + inject\n"
        "  du/dt = a * (b * Vm - u)\n"
        " if Vm >= Vmax then Vm = c and u = u + d\n"
        " Vmax = 30 mV in the paper."
    };
    static Cinfo izhikevichNrnCinfo(
            doc,
            sizeof(doc) / sizeof(string),
            initNeutralCinfo(),
            izhikevichNrnFinfos,
            sizeof(izhikevichNrnFinfos) / sizeof(Finfo*),
            ValueFtype1<IzhikevichNrn>::global(),
            schedInfo, 1);

    return &izhikevichNrnCinfo;
}

static const Cinfo* izhikevichNrnCinfo = initIzhikevichNrnCinfo();

static const Slot eventSrcSlot = initIzhikevichNrnCinfo()->getSlot("eventSrc");
static const Slot VmSrcSlot = initIzhikevichNrnCinfo()->getSlot("VmSrc");




IzhikevichNrn::IzhikevichNrn():
        alpha_(40000.0),
        beta_(5000.0),
        gamma_(140.0),
        a_(20.0),
        b_(200.0),
        c_(-0.065),
        d_(2.0),
        Vm_(-0.065),
        u_(-13.0),
        Vmax_(0.03),
        initVm_(-0.065),
        initU_(-13.0),
        sum_inject_(0.0)
{}

void IzhikevichNrn::setA(const Conn* conn, double value)
{
    static_cast<IzhikevichNrn*>(conn->data())->a_ = value;
}

double IzhikevichNrn::getA(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->a_;
}

void IzhikevichNrn::setB(const Conn* conn, double value)
{
    static_cast<IzhikevichNrn*>(conn->data())->b_ = value;
}

double IzhikevichNrn::getB(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->b_;
}
void IzhikevichNrn::setC(const Conn* conn, double value)      
{
    static_cast<IzhikevichNrn*>(conn->data())->c_ = value;
}

double IzhikevichNrn::getC(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->c_;
}                             

void IzhikevichNrn::setD(const Conn* conn, double value)       
{
    static_cast<IzhikevichNrn*>(conn->data())->d_ = value;
}

double IzhikevichNrn::getD(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->d_;
}                             
void IzhikevichNrn::setVm(const Conn* conn, double value)       
{
    static_cast<IzhikevichNrn*>(conn->data())->Vm_ = value;
}
double IzhikevichNrn::getU(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->u_;
}                             

double IzhikevichNrn::getVm(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->Vm_;
}                             

void IzhikevichNrn::setVmax(const Conn* conn, double value)    
{
    static_cast<IzhikevichNrn*>(conn->data())->Vmax_ = value;
}

double IzhikevichNrn::getVmax(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->Vmax_;
}

void IzhikevichNrn::setAlpha(const Conn* conn, double value)   
{
    static_cast<IzhikevichNrn*>(conn->data())->alpha_ = value;
}

double IzhikevichNrn::getAlpha(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->alpha_;
}

void IzhikevichNrn::setBeta(const Conn* conn, double value)
{
    static_cast<IzhikevichNrn*>(conn->data())->beta_ = value;
}

double IzhikevichNrn::getBeta(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->beta_;
}                          

void IzhikevichNrn::setGamma(const Conn* conn, double value)   
{
    static_cast<IzhikevichNrn*>(conn->data())->gamma_ = value;
}

double IzhikevichNrn::getGamma(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->gamma_;
}                         

void IzhikevichNrn::setInject(const Conn* conn, double value)
{
    static_cast<IzhikevichNrn*>(conn->data())->sum_inject_ += value;
}

double IzhikevichNrn::getInject(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->sum_inject_;
}                        

void IzhikevichNrn::setInitVm(const Conn* conn, double value)    
{
    static_cast<IzhikevichNrn*>(conn->data())->initVm_ = value;
}

double IzhikevichNrn::getInitVm(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->initVm_;
}

void IzhikevichNrn::setInitU(const Conn* conn, double value)    
{
    static_cast<IzhikevichNrn*>(conn->data())->initU_ = value;
}

double IzhikevichNrn::getInitU(Eref e)
{
    return static_cast<IzhikevichNrn*>(e.data())->initU_;
}

void IzhikevichNrn::processFunc(const Conn* conn, ProcInfo proc)
{
    double dt = proc->dt_;
    IzhikevichNrn* instance = static_cast<IzhikevichNrn*>(conn->data());
    if (instance->Vm_ >= instance->Vmax_){
        instance->Vm_ = instance->c_;
        instance->u_ += instance->d_;
        send1<double>(conn->target(), eventSrcSlot, proc->currTime_);
    } else {
        instance->Vm_ = instance->Vm_ *
                ( 1.0 + dt * (instance->alpha_ * instance->Vm_ + instance->beta_ )) +
                dt * (instance->gamma_ - instance->u_ + instance->sum_inject_);
        instance->u_ = instance->u_ +
                dt * (instance->a_ * (instance->b_ * instance->Vm_ - instance->u_));
    }
    instance->sum_inject_ = 0.0;
    send1<double>(conn->target(), VmSrcSlot, instance->Vm_);
}

void IzhikevichNrn::reinitFunc(const Conn* conn, ProcInfo proc)
{
    IzhikevichNrn* instance = static_cast<IzhikevichNrn*>(conn->data());
    instance->sum_inject_ = 0.0;
    instance->Vm_ = instance->initVm_;
    instance->u_ = instance->initU_;
}













// 
// IzhikevichNrn.cpp ends here
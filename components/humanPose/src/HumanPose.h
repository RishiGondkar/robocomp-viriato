// **********************************************************************
//
// Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************
//
// Ice version 3.7.0
//
// <auto-generated>
//
// Generated from file `HumanPose.ice'
//
// Warning: do not edit this file.
//
// </auto-generated>
//

#ifndef __HumanPose_h__
#define __HumanPose_h__

#include <IceUtil/PushDisableWarnings.h>
#include <Ice/ProxyF.h>
#include <Ice/ObjectF.h>
#include <Ice/ValueF.h>
#include <Ice/Exception.h>
#include <Ice/LocalObject.h>
#include <Ice/StreamHelpers.h>
#include <Ice/Comparable.h>
#include <Ice/Proxy.h>
#include <Ice/Object.h>
#include <Ice/GCObject.h>
#include <Ice/Value.h>
#include <Ice/Incoming.h>
#include <Ice/FactoryTableInit.h>
#include <IceUtil/ScopedArray.h>
#include <Ice/Optional.h>
#include <IceUtil/UndefSysMacros.h>

#ifndef ICE_IGNORE_VERSION
#   if ICE_INT_VERSION / 100 != 307
#       error Ice version mismatch!
#   endif
#   if ICE_INT_VERSION % 100 > 50
#       error Beta header file detected
#   endif
#   if ICE_INT_VERSION % 100 < 0
#       error Ice patch level mismatch!
#   endif
#endif

#ifdef ICE_CPP11_MAPPING // C++11 mapping

namespace RoboCompHumanPose
{

class HumanPose;
class HumanPosePrx;

}

namespace RoboCompHumanPose
{

struct Pose3D
{
    float x;
    float z;
    float ry;
    bool pos_good;
    bool rot_good;
    int confidence = 0;

    std::tuple<const float&, const float&, const float&, const bool&, const bool&, const int&> ice_tuple() const
    {
        return std::tie(x, z, ry, pos_good, rot_good, confidence);
    }
};

struct PersonType
{
    int id;
    ::RoboCompHumanPose::Pose3D pos;
    int IDcamera;

    std::tuple<const int&, const ::RoboCompHumanPose::Pose3D&, const int&> ice_tuple() const
    {
        return std::tie(id, pos, IDcamera);
    }
};

using humansDetected = ::std::vector<::RoboCompHumanPose::PersonType>;

using Ice::operator<;
using Ice::operator<=;
using Ice::operator>;
using Ice::operator>=;
using Ice::operator==;
using Ice::operator!=;

}

namespace RoboCompHumanPose
{

class HumanPose : public virtual ::Ice::Object
{
public:

    using ProxyType = HumanPosePrx;

    virtual bool ice_isA(::std::string, const ::Ice::Current&) const override;
    virtual ::std::vector<::std::string> ice_ids(const ::Ice::Current&) const override;
    virtual ::std::string ice_id(const ::Ice::Current&) const override;

    static const ::std::string& ice_staticId();

    virtual void obtainHumanPose(::RoboCompHumanPose::humansDetected, const ::Ice::Current&) = 0;
    bool _iceD_obtainHumanPose(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&) override;
};

}

namespace RoboCompHumanPose
{

class HumanPosePrx : public virtual ::Ice::Proxy<HumanPosePrx, ::Ice::ObjectPrx>
{
public:

    void obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::Context& context = Ice::noExplicitContext)
    {
        _makePromiseOutgoing<void>(true, this, &RoboCompHumanPose::HumanPosePrx::_iceI_obtainHumanPose, iceP_list_of_humans, context).get();
    }

    template<template<typename> class P = ::std::promise>
    auto obtainHumanPoseAsync(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::Context& context = Ice::noExplicitContext)
        -> decltype(::std::declval<P<void>>().get_future())
    {
        return _makePromiseOutgoing<void, P>(false, this, &RoboCompHumanPose::HumanPosePrx::_iceI_obtainHumanPose, iceP_list_of_humans, context);
    }

    ::std::function<void()>
    obtainHumanPoseAsync(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans,
                         ::std::function<void()> response,
                         ::std::function<void(::std::exception_ptr)> ex = nullptr,
                         ::std::function<void(bool)> sent = nullptr,
                         const ::Ice::Context& context = Ice::noExplicitContext)
    {
        return _makeLamdaOutgoing<void>(response, ex, sent, this, &RoboCompHumanPose::HumanPosePrx::_iceI_obtainHumanPose, iceP_list_of_humans, context);
    }

    void _iceI_obtainHumanPose(const ::std::shared_ptr<::IceInternal::OutgoingAsyncT<void>>&, const ::RoboCompHumanPose::humansDetected&, const ::Ice::Context&);

    static const ::std::string& ice_staticId();

protected:

    HumanPosePrx() = default;
    friend ::std::shared_ptr<HumanPosePrx> IceInternal::createProxy<HumanPosePrx>();

    virtual ::std::shared_ptr<::Ice::ObjectPrx> _newInstance() const override;
};

}

namespace Ice
{

template<>
struct StreamableTraits<::RoboCompHumanPose::Pose3D>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 18;
    static const bool fixedLength = true;
};

template<typename S>
struct StreamReader<::RoboCompHumanPose::Pose3D, S>
{
    static void read(S* istr, ::RoboCompHumanPose::Pose3D& v)
    {
        istr->readAll(v.x, v.z, v.ry, v.pos_good, v.rot_good, v.confidence);
    }
};

template<>
struct StreamableTraits<::RoboCompHumanPose::PersonType>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 26;
    static const bool fixedLength = true;
};

template<typename S>
struct StreamReader<::RoboCompHumanPose::PersonType, S>
{
    static void read(S* istr, ::RoboCompHumanPose::PersonType& v)
    {
        istr->readAll(v.id, v.pos, v.IDcamera);
    }
};

}

namespace RoboCompHumanPose
{

using HumanPosePtr = ::std::shared_ptr<HumanPose>;
using HumanPosePrxPtr = ::std::shared_ptr<HumanPosePrx>;

}

#else // C++98 mapping

namespace IceProxy
{

namespace RoboCompHumanPose
{

class HumanPose;
void _readProxy(::Ice::InputStream*, ::IceInternal::ProxyHandle< ::IceProxy::RoboCompHumanPose::HumanPose>&);
::IceProxy::Ice::Object* upCast(::IceProxy::RoboCompHumanPose::HumanPose*);

}

}

namespace RoboCompHumanPose
{

class HumanPose;
::Ice::Object* upCast(::RoboCompHumanPose::HumanPose*);
typedef ::IceInternal::Handle< ::RoboCompHumanPose::HumanPose> HumanPosePtr;
typedef ::IceInternal::ProxyHandle< ::IceProxy::RoboCompHumanPose::HumanPose> HumanPosePrx;
typedef HumanPosePrx HumanPosePrxPtr;
void _icePatchObjectPtr(HumanPosePtr&, const ::Ice::ObjectPtr&);

}

namespace RoboCompHumanPose
{

struct Pose3D
{
    Pose3D() :
        confidence(0)
    {
    }
    
    Pose3D(::Ice::Float iceP_x, ::Ice::Float iceP_z, ::Ice::Float iceP_ry, bool iceP_pos_good, bool iceP_rot_good, ::Ice::Int iceP_confidence) :
        x(iceP_x),
        z(iceP_z),
        ry(iceP_ry),
        pos_good(iceP_pos_good),
        rot_good(iceP_rot_good),
        confidence(iceP_confidence)
    {
    }
    

    ::Ice::Float x;
    ::Ice::Float z;
    ::Ice::Float ry;
    bool pos_good;
    bool rot_good;
    ::Ice::Int confidence;
};

struct PersonType
{
    ::Ice::Int id;
    ::RoboCompHumanPose::Pose3D pos;
    ::Ice::Int IDcamera;
};

typedef ::std::vector< ::RoboCompHumanPose::PersonType> humansDetected;

}

namespace RoboCompHumanPose
{

class Callback_HumanPose_obtainHumanPose_Base : public virtual ::IceInternal::CallbackBase { };
typedef ::IceUtil::Handle< Callback_HumanPose_obtainHumanPose_Base> Callback_HumanPose_obtainHumanPosePtr;

}

namespace IceProxy
{

namespace RoboCompHumanPose
{

class HumanPose : public virtual ::Ice::Proxy<HumanPose, ::IceProxy::Ice::Object>
{
public:

    void obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        end_obtainHumanPose(_iceI_begin_obtainHumanPose(iceP_list_of_humans, context, ::IceInternal::dummyCallback, 0, true));
    }

    ::Ice::AsyncResultPtr begin_obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::Context& context = ::Ice::noExplicitContext)
    {
        return _iceI_begin_obtainHumanPose(iceP_list_of_humans, context, ::IceInternal::dummyCallback, 0);
    }

    ::Ice::AsyncResultPtr begin_obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::CallbackPtr& del, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_obtainHumanPose(iceP_list_of_humans, ::Ice::noExplicitContext, del, cookie);
    }

    ::Ice::AsyncResultPtr begin_obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::Context& context, const ::Ice::CallbackPtr& del, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_obtainHumanPose(iceP_list_of_humans, context, del, cookie);
    }

    ::Ice::AsyncResultPtr begin_obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::RoboCompHumanPose::Callback_HumanPose_obtainHumanPosePtr& del, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_obtainHumanPose(iceP_list_of_humans, ::Ice::noExplicitContext, del, cookie);
    }

    ::Ice::AsyncResultPtr begin_obtainHumanPose(const ::RoboCompHumanPose::humansDetected& iceP_list_of_humans, const ::Ice::Context& context, const ::RoboCompHumanPose::Callback_HumanPose_obtainHumanPosePtr& del, const ::Ice::LocalObjectPtr& cookie = 0)
    {
        return _iceI_begin_obtainHumanPose(iceP_list_of_humans, context, del, cookie);
    }

    void end_obtainHumanPose(const ::Ice::AsyncResultPtr&);

private:

    ::Ice::AsyncResultPtr _iceI_begin_obtainHumanPose(const ::RoboCompHumanPose::humansDetected&, const ::Ice::Context&, const ::IceInternal::CallbackBasePtr&, const ::Ice::LocalObjectPtr& cookie = 0, bool sync = false);

public:

    static const ::std::string& ice_staticId();

protected:

    virtual ::IceProxy::Ice::Object* _newInstance() const;
};

}

}

namespace RoboCompHumanPose
{

class HumanPose : public virtual ::Ice::Object
{
public:

    typedef HumanPosePrx ProxyType;
    typedef HumanPosePtr PointerType;

    virtual ~HumanPose();

    virtual bool ice_isA(const ::std::string&, const ::Ice::Current& = ::Ice::emptyCurrent) const;
    virtual ::std::vector< ::std::string> ice_ids(const ::Ice::Current& = ::Ice::emptyCurrent) const;
    virtual const ::std::string& ice_id(const ::Ice::Current& = ::Ice::emptyCurrent) const;

    static const ::std::string& ice_staticId();

    virtual void obtainHumanPose(const ::RoboCompHumanPose::humansDetected&, const ::Ice::Current& = ::Ice::emptyCurrent) = 0;
    bool _iceD_obtainHumanPose(::IceInternal::Incoming&, const ::Ice::Current&);

    virtual bool _iceDispatch(::IceInternal::Incoming&, const ::Ice::Current&);

protected:

    virtual void _iceWriteImpl(::Ice::OutputStream*) const;
    virtual void _iceReadImpl(::Ice::InputStream*);
};

inline bool operator==(const HumanPose& lhs, const HumanPose& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) == static_cast<const ::Ice::Object&>(rhs);
}

inline bool operator<(const HumanPose& lhs, const HumanPose& rhs)
{
    return static_cast<const ::Ice::Object&>(lhs) < static_cast<const ::Ice::Object&>(rhs);
}

}

namespace Ice
{

template<>
struct StreamableTraits< ::RoboCompHumanPose::Pose3D>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 18;
    static const bool fixedLength = true;
};

template<typename S>
struct StreamWriter< ::RoboCompHumanPose::Pose3D, S>
{
    static void write(S* ostr, const ::RoboCompHumanPose::Pose3D& v)
    {
        ostr->write(v.x);
        ostr->write(v.z);
        ostr->write(v.ry);
        ostr->write(v.pos_good);
        ostr->write(v.rot_good);
        ostr->write(v.confidence);
    }
};

template<typename S>
struct StreamReader< ::RoboCompHumanPose::Pose3D, S>
{
    static void read(S* istr, ::RoboCompHumanPose::Pose3D& v)
    {
        istr->read(v.x);
        istr->read(v.z);
        istr->read(v.ry);
        istr->read(v.pos_good);
        istr->read(v.rot_good);
        istr->read(v.confidence);
    }
};

template<>
struct StreamableTraits< ::RoboCompHumanPose::PersonType>
{
    static const StreamHelperCategory helper = StreamHelperCategoryStruct;
    static const int minWireSize = 26;
    static const bool fixedLength = true;
};

template<typename S>
struct StreamWriter< ::RoboCompHumanPose::PersonType, S>
{
    static void write(S* ostr, const ::RoboCompHumanPose::PersonType& v)
    {
        ostr->write(v.id);
        ostr->write(v.pos);
        ostr->write(v.IDcamera);
    }
};

template<typename S>
struct StreamReader< ::RoboCompHumanPose::PersonType, S>
{
    static void read(S* istr, ::RoboCompHumanPose::PersonType& v)
    {
        istr->read(v.id);
        istr->read(v.pos);
        istr->read(v.IDcamera);
    }
};

}

namespace RoboCompHumanPose
{

template<class T>
class CallbackNC_HumanPose_obtainHumanPose : public Callback_HumanPose_obtainHumanPose_Base, public ::IceInternal::OnewayCallbackNC<T>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception&);
    typedef void (T::*Sent)(bool);
    typedef void (T::*Response)();

    CallbackNC_HumanPose_obtainHumanPose(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::OnewayCallbackNC<T>(obj, cb, excb, sentcb)
    {
    }
};

template<class T> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(const IceUtil::Handle<T>& instance, void (T::*cb)(), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_HumanPose_obtainHumanPose<T>(instance, cb, excb, sentcb);
}

template<class T> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(const IceUtil::Handle<T>& instance, void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_HumanPose_obtainHumanPose<T>(instance, 0, excb, sentcb);
}

template<class T> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(T* instance, void (T::*cb)(), void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_HumanPose_obtainHumanPose<T>(instance, cb, excb, sentcb);
}

template<class T> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(T* instance, void (T::*excb)(const ::Ice::Exception&), void (T::*sentcb)(bool) = 0)
{
    return new CallbackNC_HumanPose_obtainHumanPose<T>(instance, 0, excb, sentcb);
}

template<class T, typename CT>
class Callback_HumanPose_obtainHumanPose : public Callback_HumanPose_obtainHumanPose_Base, public ::IceInternal::OnewayCallback<T, CT>
{
public:

    typedef IceUtil::Handle<T> TPtr;

    typedef void (T::*Exception)(const ::Ice::Exception& , const CT&);
    typedef void (T::*Sent)(bool , const CT&);
    typedef void (T::*Response)(const CT&);

    Callback_HumanPose_obtainHumanPose(const TPtr& obj, Response cb, Exception excb, Sent sentcb)
        : ::IceInternal::OnewayCallback<T, CT>(obj, cb, excb, sentcb)
    {
    }
};

template<class T, typename CT> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(const IceUtil::Handle<T>& instance, void (T::*cb)(const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_HumanPose_obtainHumanPose<T, CT>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(const IceUtil::Handle<T>& instance, void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_HumanPose_obtainHumanPose<T, CT>(instance, 0, excb, sentcb);
}

template<class T, typename CT> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(T* instance, void (T::*cb)(const CT&), void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_HumanPose_obtainHumanPose<T, CT>(instance, cb, excb, sentcb);
}

template<class T, typename CT> Callback_HumanPose_obtainHumanPosePtr
newCallback_HumanPose_obtainHumanPose(T* instance, void (T::*excb)(const ::Ice::Exception&, const CT&), void (T::*sentcb)(bool, const CT&) = 0)
{
    return new Callback_HumanPose_obtainHumanPose<T, CT>(instance, 0, excb, sentcb);
}

}

#endif

#include <IceUtil/PopDisableWarnings.h>
#endif
#pragma once

class IProperty {
public:
    virtual ~IProperty() {}
    virtual void OnPropertyUpdated(int PropertiesId) = 0;
};

template <typename T>
class Property {
private:
    T m_value;
    int m_id;
    IProperty* m_parent;

public:
    Property(IProperty* parent, int id, const T& initialValue)
            : m_parent(parent), m_id(id), m_value(initialValue) {}

    IProperty& operator=(const T& newValue) {
        if (m_value != newValue) {
            m_value = newValue;
            if (m_parent != NULL) {
                m_parent->OnPropertyUpdated(m_id);
            }
        }
        return *this;
    }
    
    operator const T&() const { return m_value; }
    
    const T& Get() const { return m_value; }
};

template <typename TSource>
class PropertyChangedListener {
public:
    virtual ~PropertyChangedListener() {}
    virtual void OnPropertyChanged(TSource* source, int propertyId) = 0;
};

template <typename TSource>
class EventTrigger : public IProperty {
private:
    std::vector<PropertyChangedListener<TSource>*> m_listeners;

public:
    void Subscribe(PropertyChangedListener<TSource>* listener) {
        if (listener != NULL) m_listeners.push_back(listener);
    }

    void Unsubscribe(PropertyChangedListener<TSource>* listener) {
        typename std::vector<PropertyChangedListener<TSource>*>::iterator it =
                std::find(m_listeners.begin(), m_listeners.end(), listener);
        if (it != m_listeners.end()) m_listeners.erase(it);
    }

protected:
    virtual void OnPropertyUpdated(int propertyId) {
        for (size_t i = 0; i < m_listeners.size(); ++i) {
            if (m_listeners[i] != NULL) {
                m_listeners[i]->OnPropertyChanged(static_cast<TSource*>(this), propertyId);
            }
        }
    }
};

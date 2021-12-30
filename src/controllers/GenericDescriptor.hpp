#ifndef CONTROLLERS_GENERICDESCRIPTOR_HPP_
#define CONTROLLERS_GENERICDESCRIPTOR_HPP_

namespace Lego
{

class GenericDescriptor
{
public:
    GenericDescriptor(void);
    virtual ~GenericDescriptor();

    virtual void close(void);

    int descriptor(void) const;
    bool isConnected(void) const;

protected:
    int m_descriptor;

    void resetDescriptor(void);
};

} /* namespace Lego */

#endif /* CONTROLLERS_GENERICDESCRIPTOR_HPP_ */

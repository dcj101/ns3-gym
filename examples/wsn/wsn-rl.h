#ifndef WSN_RL_H
#define WSN_RL_H

#include "ns3/lr-wpan-csmaca.h"

namespace ns3
{

class WsnRl : public Object
{

public:
    static TypeId GetTypeId (void);

    WsnRl ();
    WsnRl (const WsnRl& wsnrl);
    ~WsnRl ();
    

private:
    float m_reward {1.0};
    float m_penalty {-100.0};
};

}

#endif
#include "wsn-rl.h"

namespace ns3
{
NS_LOG_COMPONENT_DEFINE ("ns3::WsnRl");

NS_OBJECT_ENSURE_REGISTERED (WsnRl);

TypeId 
WsnRl::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::WsnRl")
    .SetParent<Object> ()
    .AddConstructor<WsnRl> ()
    .AddAttribute ("Reward", "Reward",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&WsnRl::m_reward),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("Penalty", "Penalty",
                   DoubleValue (-10.0),
                   MakeDoubleAccessor (&WsnRl::m_penalty),
                   MakeDoubleChecker<double> ())
    ;
    return tid;
}

WsnRl::WsnRl(void)
{
    m_wsnRlGymEnv = 0;
}

WsnRl::~WsnRl(void)
{
    m_wsnRlGymEnv = 0;
}

uint32_t
WsnRl::GetBackoffRl(uint32_t lostPacketRadio, float sendRate, float Delay)
{
    if(!m_wsnRlGymEnv)
    {
        CreateGymEnv();
    }
    uint32_t newBackoff = 0;
    if(m_wsnRlGymEnv) 
    {
        newBackoff = m_wsnRlGymEnv->GetBackoffRl(lostPacketRadio,
                                                 sendRate,
                                                 Delay );
    }
    return newBackoff;
}

void
WsnRl::CreateGymEnv()
{
    Ptr<WsnRlGymEnv> env = CreateObject<WsnRlGymEnv>();
    env->SetReward(m_reward);
    env->SetPenalty(m_penalty);
    m_wsnRlGymEnv = env;
}

}
#include <ns3/log.h>
#include <ns3/core-module.h>
#include <ns3/lr-wpan-module.h>
#include <ns3/propagation-loss-model.h>
#include <ns3/propagation-delay-model.h>
#include <ns3/simulator.h>
#include <ns3/single-model-spectrum-channel.h>
#include <ns3/constant-position-mobility-model.h>
#include <ns3/packet.h>
#include <ns3/wsn-module.h>
#include <ns3/wsn-flow-monitor-helper.h>
#include <ns3/lr-wpan-flow-helper.h>
#include <iostream>
#include "ns3/enum.h"
#include "ns3/event-id.h"
#include "wsn-rl-env.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;
using namespace std;
NS_LOG_COMPONENT_DEFINE ("testexample");

void Test(Ptr<WsnNwkProtocol> from, Ptr<WsnNwkProtocol> to)
{
    Simulator::ScheduleWithContext(1,Seconds(0.0),&WsnNwkProtocol::Send,
                      from,from->GetNwkShortAddress(),to->GetNwkShortAddress(),
                      Create<Packet>(50),
                      NwkHeader::NWK_FRAME_DATA,WsnNwkPayload::WSN_PL_NONE);
    // Simulator::ScheduleWithContext(2,Seconds(1.0),&WsnNwkProtocol::Send,
    //                   to,to->GetNwkShortAddress(),from->GetNwkShortAddress(),
    //                   Create<Packet>(31),
    //                   NwkHeader::NWK_FRAME_DATA,0);
}

int main()
{
    // LogComponentEnable ("testexample", LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_INFO));
    // LogComponentEnableAll (LOG_PREFIX_TIME);
    // LogComponentEnableAll (LOG_PREFIX_FUNC);
    // LogComponentEnable ("LrWpanMac", LOG_LEVEL_ALL);
    // LogComponentEnable ("WsnFlowProbe", LOG_LEVEL_ALL);
    // LogComponentEnable ("LrWpanPhy", LOG_LEVEL_ALL);
    // LogComponentEnable ("LrWpanNetDevice", LOG_LEVEL_ALL);
    LogComponentEnable ("WsnNwkProtocol", LOG_LEVEL_ALL);
    // LogComponentEnable ("NwkHeader", LOG_LEVEL_ALL);
    // LogComponentEnable ("WsnAddressAllocator", LOG_LEVEL_ALL);
    // LogComponentEnable ("Node", LOG_LEVEL_ALL);
    // LogComponentEnable("OpenGymInterface",LOG_LEVEL_ALL);
    // LogComponentEnable("OpenGymDataContainer",LOG_LEVEL_ALL);
    // LogComponentEnable("OpenGymSpace",LOG_LEVEL_ALL);
    // LogComponentEnable("OpenGymEnv",LOG_LEVEL_ALL);
    // LogComponentEnable ("LrWpanCsmaCa", LOG_LEVEL_ALL);
    LogComponentEnable ("ns3::WsnRlGymEnv", LOG_LEVEL_ALL);

    uint32_t openGymPort = 5555;
    // OpenGym Env --- has to be created before any other thing
    Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
    Ptr<WsnRlGymEnv> myGymEnv = CreateObject<WsnRlGymEnv> ();
    myGymEnv->SetIsFedLearning(1);
    myGymEnv->SetOpenGymInterface(openGymInterface);
    
    uint32_t openGymPort1 = 5556;
    // OpenGym Env --- has to be created before any other thing
    Ptr<OpenGymInterface> openGymInterface1 = CreateObject<OpenGymInterface> (openGymPort1);
    Ptr<WsnRlGymEnv> myGymEnv1 = CreateObject<WsnRlGymEnv> ();
    myGymEnv1->SetIsFedLearning(1);
    myGymEnv1->SetOpenGymInterface(openGymInterface1);

    // 设置入网分配器的分配规则
    WsnAddressAllocator::Get ()->SetWsnAddressAllocator(5,3,3);

    LrWpanHelper lrWpanHelper;

    // Create 2 nodes, and a NetDevice for each one
    Ptr<Node> n0 = CreateObject <Node> ();
    Ptr<Node> n1 = CreateObject <Node> ();
    Ptr<Node> n2 = CreateObject <Node> ();
    Ptr<Node> n3 = CreateObject <Node> ();
    Ptr<Node> n4 = CreateObject <Node> ();

    Ptr<LrWpanNetDevice> dev0 = CreateObject<LrWpanNetDevice> ();
    Ptr<LrWpanNetDevice> dev1 = CreateObject<LrWpanNetDevice> ();
    Ptr<LrWpanNetDevice> dev2 = CreateObject<LrWpanNetDevice> ();
    Ptr<LrWpanNetDevice> dev3 = CreateObject<LrWpanNetDevice> ();
    Ptr<LrWpanNetDevice> dev4 = CreateObject<LrWpanNetDevice> ();

    string mac480 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
    string mac481 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
    string mac482 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
    string mac483 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
    string mac484 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();


    string mac640 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac480);
    std::cout << mac640 << "\n";
    string mac641 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac481);
    std::cout << mac641 << "\n";
    string mac642 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac482);
    std::cout << mac642 << "\n";
    string mac643 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac483);
    std::cout << mac643 << "\n";
    string mac644 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac484);
    std::cout << mac644 << "\n";


    dev0->GetMac()->SetExtendedAddress (Mac64Address (mac640.c_str()));
    dev1->GetMac()->SetExtendedAddress (Mac64Address (mac641.c_str()));
    dev2->GetMac()->SetExtendedAddress (Mac64Address (mac642.c_str()));
    dev3->GetMac()->SetExtendedAddress (Mac64Address (mac643.c_str()));
    dev4->GetMac()->SetExtendedAddress (Mac64Address (mac644.c_str()));

    Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
    // 距离传输模型
    Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
    // 介质传输模型 
    Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();

    channel->AddPropagationLossModel (propModel);
    channel->SetPropagationDelayModel (delayModel);

    dev0->SetChannel (channel);
    dev1->SetChannel (channel);
    dev2->SetChannel (channel);
    dev3->SetChannel (channel);
    dev4->SetChannel (channel);

    WsnNwkProtocolHelper helper;
    // Ptr<WsnNwkProtocol> nwk0 = CreateObject<WsnNwkProtocol>(NODE_TYPE::COOR);
    helper.CreateAndAggregateObjectFromTypeId(n0,"ns3::WsnNwkProtocol");
    Ptr<WsnNwkProtocol> nwk0 = n0->GetObject<WsnNwkProtocol>();
    nwk0->SetNodeType(NODE_TYPE::COOR);
    
    // Ptr<WsnNwkProtocol> nwk1 = CreateObject<WsnNwkProtocol>(NODE_TYPE::EDGE);
    helper.CreateAndAggregateObjectFromTypeId(n1,"ns3::WsnNwkProtocol");
    Ptr<WsnNwkProtocol> nwk1 = n1->GetObject<WsnNwkProtocol>();
    nwk1->SetNodeType(NODE_TYPE::EDGE);

    // Ptr<WsnNwkProtocol> nwk2 = CreateObject<WsnNwkProtocol>(NODE_TYPE::EDGE);
    helper.CreateAndAggregateObjectFromTypeId(n2,"ns3::WsnNwkProtocol");
    Ptr<WsnNwkProtocol> nwk2 = n2->GetObject<WsnNwkProtocol>();
    nwk2->SetNodeType(NODE_TYPE::EDGE);
    nwk2->SetGetModelCallBack(MakeCallback(&WsnRlGymEnv::GetMyModel,myGymEnv));
    nwk2->SetRecvModelCallBack(MakeCallback(&WsnRlGymEnv::RecvModel,myGymEnv));

    // Ptr<WsnNwkProtocol> nwk3 = CreateObject<WsnNwkProtocol>(NODE_TYPE::ROUTE);
    helper.CreateAndAggregateObjectFromTypeId(n3,"ns3::WsnNwkProtocol");
    Ptr<WsnNwkProtocol> nwk3 = n3->GetObject<WsnNwkProtocol>();
    nwk3->SetNodeType(NODE_TYPE::ROUTE);
    nwk3->SetGetModelCallBack(MakeCallback(&WsnRlGymEnv::GetMyModel,myGymEnv1));
    nwk3->SetRecvModelCallBack(MakeCallback(&WsnRlGymEnv::RecvModel,myGymEnv1));


    //Ptr<WsnNwkProtocol> nwk4 = CreateObject<WsnNwkProtocol>(NODE_TYPE::EDGE);
    helper.CreateAndAggregateObjectFromTypeId(n4,"ns3::WsnNwkProtocol");
    Ptr<WsnNwkProtocol> nwk4 = n4->GetObject<WsnNwkProtocol>();
    nwk4->SetNodeType(NODE_TYPE::EDGE);


    nwk0->Assign(dev0);
    nwk1->Assign(dev1);
    nwk2->Assign(dev2);
    nwk3->Assign(dev3);
    nwk4->Assign(dev4);

    nwk0->Install(n0);
    nwk1->Install(n1);
    nwk2->Install(n2);
    nwk3->Install(n3);
    nwk4->Install(n4);

    n0->AddDevice (dev0);
    n1->AddDevice (dev1);
    n2->AddDevice (dev2);
    n3->AddDevice (dev3);
    n4->AddDevice (dev4);


    Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
    sender0Mobility->SetPosition (Vector (0,0,0));
    dev0->GetPhy ()->SetMobility (sender0Mobility);
    

    Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
    sender1Mobility->SetPosition (Vector (0,10,0)); //10 m distance
    dev1->GetPhy ()->SetMobility (sender1Mobility);

    
    Ptr<ConstantPositionMobilityModel> sender2Mobility = CreateObject<ConstantPositionMobilityModel> ();
    sender2Mobility->SetPosition (Vector (10,0,0));
    dev2->GetPhy ()->SetMobility (sender2Mobility);
    dev2->GetCsmaCa ()->SetLrWpanNwkBackOffRl(MakeCallback(&WsnRlGymEnv::GetBackoffRl,myGymEnv));
    
    Ptr<ConstantPositionMobilityModel> sender3Mobility = CreateObject<ConstantPositionMobilityModel> ();
    sender3Mobility->SetPosition (Vector (0,0,10)); //10 m distance
    dev3->GetPhy ()->SetMobility (sender3Mobility);
    dev3->GetCsmaCa ()->SetLrWpanNwkBackOffRl(MakeCallback(&WsnRlGymEnv::GetBackoffRl,myGymEnv1));

    
    Ptr<ConstantPositionMobilityModel> sender4Mobility = CreateObject<ConstantPositionMobilityModel> ();
    sender4Mobility->SetPosition (Vector (0,10,10)); //10 m distance
    dev4->GetPhy ()->SetMobility (sender4Mobility);


    Ptr<FlowMonitor> monitor = LrWpanFlowMonitorHelper::Get ()->Install(dev0);
    LrWpanFlowMonitorHelper::Get ()->Install(dev1);
    LrWpanFlowMonitorHelper::Get ()->Install(dev2);
    LrWpanFlowMonitorHelper::Get ()->Install(dev3);
    LrWpanFlowMonitorHelper::Get ()->Install(dev4);

    Simulator::Schedule(Seconds(0.0),&WsnNwkProtocol::JoinRequest,
                        nwk0,nwk1);
    // nwk0->JoinRequest(NODE_TYPE::COOR,NULL);
    Simulator::Schedule(Seconds(25.0),&WsnNwkProtocol::JoinRequest,
                        nwk3,nwk0);
    // // nwk1->JoinRequest(NODE_TYPE::EDGE,nwk1);

    Simulator::Schedule(Seconds(40.0),&WsnNwkProtocol::JoinRequest,
                        nwk1,nwk0);

    Simulator::Schedule(Seconds(55.0),&WsnNwkProtocol::JoinRequest,
                        nwk2,nwk3);

    Simulator::Schedule(Seconds(60.0),&WsnNwkProtocol::JoinRequest,
                        nwk4,nwk3);        

    double sendtime = 100;
    Simulator::Schedule(Seconds(sendtime+5),&WsnNwkProtocol::GetModel,nwk2);
    Simulator::Schedule(Seconds(sendtime+5),&WsnNwkProtocol::GetModel,nwk3);
    for(int i = 0; i < 3800; i+=6)
    { 
        Ptr<UniformRandomVariable> uniformRandomVariable = CreateObject<UniformRandomVariable> ();;
        double delay = uniformRandomVariable->GetValue (0, 0.1);
        Simulator::Schedule(Seconds(sendtime+(i+0)*delay),&Test,nwk2,nwk1);
        Simulator::Schedule(Seconds(sendtime+(i+1)*delay),&Test,nwk1,nwk2);
        Simulator::Schedule(Seconds(sendtime+(i+2)*delay),&Test,nwk4,nwk1);
        Simulator::Schedule(Seconds(sendtime+(i+3)*delay),&Test,nwk1,nwk4);
        Simulator::Schedule(Seconds(sendtime+(i+4)*delay),&Test,nwk2,nwk4);    
        Simulator::Schedule(Seconds(sendtime+(i+5)*delay),&Test,nwk4,nwk2);
    }



    bool TraceMetric = 1;

    Simulator::Stop(Seconds(200));
    Simulator::Run ();

    uint32_t SentPackets = 0;
        uint32_t ReceivedPackets = 0;
        uint32_t LostPackets = 0;
    if (TraceMetric)
        {
            int j=0;
            float AvgThroughput = 0;
            Time Jitter;
            Time Delay;

            Ptr<LrWpanFlowClassifier> classifier = DynamicCast<LrWpanFlowClassifier> (LrWpanFlowMonitorHelper::Get() ->GetClassifier ());

            std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

            for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
            {
            LrWpanFlowClassifier::TwoTuple t = classifier->FindFlow (iter->first);
            NS_LOG_UNCOND("----Flow ID:" <<iter->first);
            NS_LOG_UNCOND("Src Addr" <<t.sourceAddress << "Dst Addr "<< t.destinationAddress);
            NS_LOG_UNCOND("Sent Packets=" <<iter->second.txPackets);
            NS_LOG_UNCOND("Received Packets =" <<iter->second.rxPackets);
            NS_LOG_UNCOND("Lost Packets =" <<iter->second.txPackets-iter->second.rxPackets);
            NS_LOG_UNCOND("Packet delivery ratio =" <<(double)iter->second.rxPackets*100/iter->second.txPackets << "%");
            NS_LOG_UNCOND("Packet loss ratio =" << (double)(iter->second.txPackets-iter->second.rxPackets)*100/iter->second.txPackets << "%");
            NS_LOG_UNCOND("Delay =" <<iter->second.delaySum);
            NS_LOG_UNCOND("Jitter =" <<iter->second.jitterSum);
            NS_LOG_UNCOND("Throughput =" <<iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024<<"Kbps\n\n\n");

            SentPackets = SentPackets +(iter->second.txPackets);
            ReceivedPackets = ReceivedPackets + (iter->second.rxPackets);
            LostPackets = LostPackets + (iter->second.txPackets-iter->second.rxPackets);
            AvgThroughput = AvgThroughput + (iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024);
            Delay = Delay + (iter->second.delaySum);
            Jitter = Jitter + (iter->second.jitterSum);

            j = j + 1;

            }

            AvgThroughput = AvgThroughput/j;
            NS_LOG_UNCOND("--------Total Results of the simulation----------"<<std::endl);
            NS_LOG_UNCOND("Total sent packets  =" << SentPackets);
            NS_LOG_UNCOND("Total Received Packets =" << ReceivedPackets);
            NS_LOG_UNCOND("Total Lost Packets =" << LostPackets);
            NS_LOG_UNCOND("Packet Loss ratio =" << ((double)(LostPackets*100)/SentPackets)<< "%");
            NS_LOG_UNCOND("Packet delivery ratio =" << ((double)(ReceivedPackets*100)/SentPackets)<< "%");
            NS_LOG_UNCOND("Average Throughput =" << AvgThroughput<< "Kbps");
            NS_LOG_UNCOND("End to End Delay =" << Delay);
            NS_LOG_UNCOND("End to End Jitter delay =" << Jitter); 
            NS_LOG_UNCOND("Total Flod id " << j);
            monitor->SerializeToXmlFile("manet-routing.xml", true, true );
        }

    Simulator::Destroy ();
    openGymInterface->NotifySimulationEnd();
    openGymInterface1->NotifySimulationEnd();
    return 0;

}

// #include <ns3/log.h>
// #include <ns3/core-module.h>
// #include <ns3/lr-wpan-module.h>
// #include <ns3/propagation-loss-model.h>
// #include <ns3/propagation-delay-model.h>
// #include <ns3/simulator.h>
// #include <ns3/single-model-spectrum-channel.h>
// #include <ns3/constant-position-mobility-model.h>
// #include <ns3/packet.h>
// #include <ns3/wsn-module.h>
// #include <ns3/wsn-flow-monitor-helper.h>
// #include <ns3/lr-wpan-flow-helper.h>
// #include <iostream>
// #include "ns3/random-variable-stream.h"

// using namespace ns3;
// using namespace std;
// NS_LOG_COMPONENT_DEFINE ("testexample");

// void Test(Ptr<WsnNwkProtocol> from, Ptr<WsnNwkProtocol> to)
// {
//     Simulator::ScheduleWithContext(1,Seconds(0.0),&WsnNwkProtocol::Send,
//                       from,from->GetNwkShortAddress(),to->GetNwkShortAddress(),
//                       Create<Packet>(11),
//                       NwkHeader::NWK_FRAME_DATA,WsnNwkPayload::WSN_PL_NONE);
//     // Simulator::ScheduleWithContext(2,Seconds(1.0),&WsnNwkProtocol::Send,
//     //                   to,to->GetNwkShortAddress(),from->GetNwkShortAddress(),
//     //                   Create<Packet>(31),
//     //                   NwkHeader::NWK_FRAME_DATA,0);
// }

// int main()
// {
//   // LogComponentEnable ("testexample", LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE | LOG_LEVEL_INFO));
//   // LogComponentEnableAll (LOG_PREFIX_TIME);
//   // LogComponentEnableAll (LOG_PREFIX_FUNC);
//   // LogComponentEnable ("LrWpanMac", LOG_LEVEL_ALL);
//   // LogComponentEnable ("WsnFlowProbe", LOG_LEVEL_ALL);
//   // LogComponentEnable ("LrWpanPhy", LOG_LEVEL_ALL);
// //   LogComponentEnable ("LrWpanCsmaCa", LOG_LEVEL_ALL);
//   // LogComponentEnable ("LrWpanNetDevice", LOG_LEVEL_ALL);
//   LogComponentEnable ("WsnNwkProtocol", LOG_LEVEL_ALL);
//   // LogComponentEnable ("NwkHeader", LOG_LEVEL_ALL);
//   // LogComponentEnable ("WsnAddressAllocator", LOG_LEVEL_ALL);
//   // LogComponentEnable ("Node", LOG_LEVEL_ALL);
// //   LogComponentEnable ("LrWpanFlowProbe", LOG_LEVEL_ALL);
//     uint32_t openGymPort = 5555;
//     // OpenGym Env --- has to be created before any other thing
//     Ptr<OpenGymInterface> openGymInterface = CreateObject<OpenGymInterface> (openGymPort);
//     Ptr<WsnRlGymEnv> myGymEnv = CreateObject<WsnRlGymEnv> ();
//     // myGymEnv->SetIsFedLearning(1);
//     myGymEnv->SetOpenGymInterface(openGymInterface);
    
//     uint32_t openGymPort1 = 5556;
//     // OpenGym Env --- has to be created before any other thing
//     Ptr<OpenGymInterface> openGymInterface1 = CreateObject<OpenGymInterface> (openGymPort1);
//     Ptr<WsnRlGymEnv> myGymEnv1 = CreateObject<WsnRlGymEnv> ();
//     // myGymEnv1->SetIsFedLearning(1);
//     myGymEnv1->SetOpenGymInterface(openGymInterface1);
//   // 设置入网分配器的分配规则
//   WsnAddressAllocator::Get ()->SetWsnAddressAllocator(10,5,5);

//   LrWpanHelper lrWpanHelper;

//   // Create 2 nodes, and a NetDevice for each one
//   Ptr<Node> n0 = CreateObject <Node> ();
//   Ptr<Node> n1 = CreateObject <Node> ();
//   Ptr<Node> n2 = CreateObject <Node> ();
//   Ptr<Node> n3 = CreateObject <Node> ();
//   Ptr<Node> n4 = CreateObject <Node> ();
//   Ptr<Node> n5 = CreateObject <Node> ();
//   Ptr<Node> n6 = CreateObject <Node> ();
//   Ptr<Node> n7 = CreateObject <Node> ();
//   Ptr<Node> n8 = CreateObject <Node> ();
//   Ptr<Node> n9 = CreateObject <Node> ();
//   Ptr<Node> n10 = CreateObject <Node> ();
//   Ptr<Node> n11 = CreateObject <Node> ();
//   Ptr<Node> n12 = CreateObject <Node> ();

//   Ptr<LrWpanNetDevice> dev0 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev1 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev2 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev3 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev4 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev5 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev6 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev7 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev8 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev9 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev10 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev11 = CreateObject<LrWpanNetDevice> ();
//   Ptr<LrWpanNetDevice> dev12 = CreateObject<LrWpanNetDevice> ();

//   string mac480 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac481 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac482 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac483 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac484 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac485 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac486 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac487 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac488 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac489 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac4810 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac4811 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();
//   string mac4812 = WsnAddressAllocator::Get ()->AllocateRandMac48Address();


//   string mac640 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac480);
//   std::cout << mac640 << "\n";
//   string mac641 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac481);
//   std::cout << mac641 << "\n";
//   string mac642 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac482);
//   std::cout << mac642 << "\n";
//   string mac643 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac483);
//   std::cout << mac643 << "\n";
//   string mac644 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac484);
//   string mac645 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac485);
//   string mac646 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac486);
//   string mac647 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac487);
//   string mac648 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac488);
//   string mac649 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac489);
//   string mac6410 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac4810);
//   string mac6411 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac4811);
//   string mac6412 = WsnAddressAllocator::Get ()->AnalysisMac48AddresstoEUI64(mac4812);
//   std::cout << mac644 << "\n";


//   dev0->GetMac()->SetExtendedAddress (Mac64Address (mac640.c_str()));
//   dev1->GetMac()->SetExtendedAddress (Mac64Address (mac641.c_str()));
//   dev2->GetMac()->SetExtendedAddress (Mac64Address (mac642.c_str()));
//   dev3->GetMac()->SetExtendedAddress (Mac64Address (mac643.c_str()));
//   dev4->GetMac()->SetExtendedAddress (Mac64Address (mac644.c_str()));
//   dev5->GetMac()->SetExtendedAddress (Mac64Address (mac645.c_str()));
//   dev6->GetMac()->SetExtendedAddress (Mac64Address (mac646.c_str()));
//   dev7->GetMac()->SetExtendedAddress (Mac64Address (mac647.c_str()));
//   dev8->GetMac()->SetExtendedAddress (Mac64Address (mac648.c_str()));
//   dev9->GetMac()->SetExtendedAddress (Mac64Address (mac649.c_str()));
//   dev10->GetMac()->SetExtendedAddress (Mac64Address (mac6410.c_str()));
//   dev11->GetMac()->SetExtendedAddress (Mac64Address (mac6411.c_str()));
//   dev12->GetMac()->SetExtendedAddress (Mac64Address (mac6412.c_str()));


//   Ptr<SingleModelSpectrumChannel> channel = CreateObject<SingleModelSpectrumChannel> ();
//   // 距离传输模型
//   Ptr<LogDistancePropagationLossModel> propModel = CreateObject<LogDistancePropagationLossModel> ();
//   // 介质传输模型 
//   Ptr<ConstantSpeedPropagationDelayModel> delayModel = CreateObject<ConstantSpeedPropagationDelayModel> ();

//   channel->AddPropagationLossModel (propModel);
//   channel->SetPropagationDelayModel (delayModel);

//   dev0->SetChannel (channel);
//   dev1->SetChannel (channel);
//   dev2->SetChannel (channel);
//   dev3->SetChannel (channel);
//   dev4->SetChannel (channel);
//   dev5->SetChannel(channel);
//   dev6->SetChannel(channel);
//   dev7->SetChannel(channel);
//   dev8->SetChannel(channel);
//   dev9->SetChannel(channel);
//   dev10->SetChannel(channel);
//   dev11->SetChannel(channel);
//   dev12->SetChannel(channel);
//   WsnNwkProtocolHelper helper;
//   // Ptr<WsnNwkProtocol> nwk0 = CreateObject<WsnNwkProtocol>(NODE_TYPE::COOR);
//   helper.CreateAndAggregateObjectFromTypeId(n0,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk0 = n0->GetObject<WsnNwkProtocol>();
//   nwk0->SetNodeType(NODE_TYPE::COOR);
  
//   // Ptr<WsnNwkProtocol> nwk1 = CreateObject<WsnNwkProtocol>(NODE_TYPE::EDGE);
//   helper.CreateAndAggregateObjectFromTypeId(n1,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk1 = n1->GetObject<WsnNwkProtocol>();
//   nwk1->SetNodeType(NODE_TYPE::ROUTE);

//   // Ptr<WsnNwkProtocol> nwk2 = CreateObject<WsnNwkProtocol>(NODE_TYPE::EDGE);
//   helper.CreateAndAggregateObjectFromTypeId(n2,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk2 = n2->GetObject<WsnNwkProtocol>();
//   nwk2->SetNodeType(NODE_TYPE::EDGE);

//   // Ptr<WsnNwkProtocol> nwk3 = CreateObject<WsnNwkProtocol>(NODE_TYPE::ROUTE);
//   helper.CreateAndAggregateObjectFromTypeId(n3,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk3 = n3->GetObject<WsnNwkProtocol>();
//   nwk3->SetNodeType(NODE_TYPE::ROUTE);

  
//   //Ptr<WsnNwkProtocol> nwk4 = CreateObject<WsnNwkProtocol>(NODE_TYPE::EDGE);
//   helper.CreateAndAggregateObjectFromTypeId(n4,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk4 = n4->GetObject<WsnNwkProtocol>();
//   nwk4->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n5,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk5 = n5->GetObject<WsnNwkProtocol>();
//   nwk5->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n6,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk6 = n6->GetObject<WsnNwkProtocol>();
//   nwk6->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n7,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk7 = n7->GetObject<WsnNwkProtocol>();
//   nwk7->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n8,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk8 = n8->GetObject<WsnNwkProtocol>();
//   nwk8->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n9,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk9 = n9->GetObject<WsnNwkProtocol>();
//   nwk9->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n10,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk10 = n10->GetObject<WsnNwkProtocol>();
//   nwk10->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n11,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk11 = n11->GetObject<WsnNwkProtocol>();
//   nwk11->SetNodeType(NODE_TYPE::EDGE);

//   helper.CreateAndAggregateObjectFromTypeId(n12,"ns3::WsnNwkProtocol");
//   Ptr<WsnNwkProtocol> nwk12 = n12->GetObject<WsnNwkProtocol>();
//   nwk12->SetNodeType(NODE_TYPE::EDGE);


//   nwk0->Assign(dev0);
//   nwk1->Assign(dev1);
//   nwk2->Assign(dev2);
//   nwk3->Assign(dev3);
//   nwk4->Assign(dev4);
//   nwk5->Assign(dev5);
//   nwk6->Assign(dev6);
//   nwk7->Assign(dev7);
//   nwk8->Assign(dev8);
//   nwk9->Assign(dev9);
//   nwk10->Assign(dev10);
//   nwk11->Assign(dev11);
//   nwk12->Assign(dev12);
  
//   nwk0->Install(n0);
//   nwk1->Install(n1);
//   nwk2->Install(n2);
//   nwk3->Install(n3);
//   nwk4->Install(n4);
//   nwk5->Install(n5);
//   nwk6->Install(n6);
//   nwk7->Install(n7);
//   nwk8->Install(n8);
//   nwk9->Install(n9);
//   nwk10->Install(n10);
//   nwk11->Install(n11);
//   nwk12->Install(n12);

//   n0->AddDevice(dev0);
//   n1->AddDevice(dev1);
//   n2->AddDevice(dev2);
//   n3->AddDevice(dev3);
//   n4->AddDevice(dev4);
//   n5->AddDevice(dev5);
//   n6->AddDevice(dev6);
//   n7->AddDevice(dev7);
//   n8->AddDevice(dev8);
//   n9->AddDevice(dev9);
//   n10->AddDevice(dev10);
//   n11->AddDevice(dev11);
//   n12->AddDevice(dev12);

//   dev1->GetCsmaCa ()->SetLrWpanNwkBackOffRl(MakeCallback(&WsnRlGymEnv::GetBackoffRl,myGymEnv));
//   dev3->GetCsmaCa ()->SetLrWpanNwkBackOffRl(MakeCallback(&WsnRlGymEnv::GetBackoffRl,myGymEnv1));


//   // Ptr<ConstantPositionMobilityModel> sender0Mobility = CreateObject<ConstantPositionMobilityModel> ();
//   // sender0Mobility->SetPosition (Vector (0,0,0));
//   // dev0->GetPhy ()->SetMobility (sender0Mobility);

//   // Ptr<ConstantPositionMobilityModel> sender1Mobility = CreateObject<ConstantPositionMobilityModel> ();
//   // sender1Mobility->SetPosition (Vector (0,30,0)); //10 m distance
//   // dev1->GetPhy ()->SetMobility (sender1Mobility);

//   // Ptr<ConstantPositionMobilityModel> sender2Mobility = CreateObject<ConstantPositionMobilityModel> ();
//   // sender2Mobility->SetPosition (Vector (30,0,0));
//   // dev2->GetPhy ()->SetMobility (sender2Mobility);

//   // Ptr<ConstantPositionMobilityModel> sender3Mobility = CreateObject<ConstantPositionMobilityModel> ();
//   // sender3Mobility->SetPosition (Vector (0,0,30)); //10 m distance
//   // dev3->GetPhy ()->SetMobility (sender3Mobility);

//   // Ptr<ConstantPositionMobilityModel> sender4Mobility = CreateObject<ConstantPositionMobilityModel> ();
//   // sender4Mobility->SetPosition (Vector (0,30,30)); //10 m distance
//   // dev4->GetPhy ()->SetMobility (sender4Mobility);


//   Ptr<FlowMonitor> monitor = LrWpanFlowMonitorHelper::Get ()->Install(dev0);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev1);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev2);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev3);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev4);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev5);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev6);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev7);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev8);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev9);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev10);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev11);
//   LrWpanFlowMonitorHelper::Get ()->Install(dev12);


//   Simulator::Schedule(Seconds(0.0),&WsnNwkProtocol::JoinRequest,
//                       nwk0,nwk1);
//   // nwk0->JoinRequest(NODE_TYPE::COOR,NULL);
//   Simulator::Schedule(Seconds(15.0),&WsnNwkProtocol::JoinRequest,
//                       nwk3,nwk0);
//   // // nwk1->JoinRequest(NODE_TYPE::EDGE,nwk1);

//   Simulator::Schedule(Seconds(30.0),&WsnNwkProtocol::JoinRequest,
//                       nwk1,nwk0);

//   Simulator::Schedule(Seconds(45.0),&WsnNwkProtocol::JoinRequest,
//                       nwk2,nwk3);

//   Simulator::Schedule(Seconds(60.0),&WsnNwkProtocol::JoinRequest,
//                       nwk4,nwk3);        

//   Simulator::Schedule(Seconds(75.0),&WsnNwkProtocol::JoinRequest,
//                       nwk5,nwk3);  

//   Simulator::Schedule(Seconds(90.0),&WsnNwkProtocol::JoinRequest,
//                       nwk6,nwk3); 

//   Simulator::Schedule(Seconds(105.0),&WsnNwkProtocol::JoinRequest,
//                       nwk7,nwk3); 
                
//   Simulator::Schedule(Seconds(120.0),&WsnNwkProtocol::JoinRequest,
//                       nwk8,nwk1); 

//   Simulator::Schedule(Seconds(135.0),&WsnNwkProtocol::JoinRequest,
//                       nwk9,nwk1);

//   Simulator::Schedule(Seconds(150.0),&WsnNwkProtocol::JoinRequest,
//                       nwk10,nwk1);

//   Simulator::Schedule(Seconds(165.0),&WsnNwkProtocol::JoinRequest,
//                       nwk11,nwk1);

//   Simulator::Schedule(Seconds(180.0),&WsnNwkProtocol::JoinRequest,
//                       nwk12,nwk1);

//   double sendtime = 200;

//   for(int i = 0; i < 8500; i+=6)
//   { 
//     Ptr<UniformRandomVariable> uniformRandomVariable = CreateObject<UniformRandomVariable> ();;
//     double delay = uniformRandomVariable->GetValue (0, 0.1);
//     Simulator::Schedule(Seconds(sendtime+(i+0)*delay),&Test,nwk2,nwk1);
//     Simulator::Schedule(Seconds(sendtime+(i+1)*delay),&Test,nwk1,nwk2);
//     Simulator::Schedule(Seconds(sendtime+(i+2)*delay),&Test,nwk4,nwk1);
//     Simulator::Schedule(Seconds(sendtime+(i+3)*delay),&Test,nwk1,nwk4);
//     Simulator::Schedule(Seconds(sendtime+(i+4)*delay),&Test,nwk2,nwk4);    
//     Simulator::Schedule(Seconds(sendtime+(i+5)*delay),&Test,nwk5,nwk6);    
//     Simulator::Schedule(Seconds(sendtime+(i+5)*delay),&Test,nwk5,nwk8);    
//     Simulator::Schedule(Seconds(sendtime+(i+5)*delay),&Test,nwk4,nwk10);    
//     Simulator::Schedule(Seconds(sendtime+(i+5)*delay),&Test,nwk7,nwk12);    
//     Simulator::Schedule(Seconds(sendtime+(i+5)*delay),&Test,nwk6,nwk11);    
//   }



//   bool TraceMetric = 1;

//   Simulator::Stop(Seconds(300));
//   Simulator::Run ();

//   uint32_t SentPackets = 0;
// 	uint32_t ReceivedPackets = 0;
// 	uint32_t LostPackets = 0;
//   if (TraceMetric)
// 	{
// 		int j=0;
// 		float AvgThroughput = 0;
// 		Time Jitter;
// 		Time Delay;

// 		Ptr<LrWpanFlowClassifier> classifier = DynamicCast<LrWpanFlowClassifier> (LrWpanFlowMonitorHelper::Get() ->GetClassifier ());

//     std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();

// 		for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator iter = stats.begin (); iter != stats.end (); ++iter)
// 		{
//         LrWpanFlowClassifier::TwoTuple t = classifier->FindFlow (iter->first);
//         // if(t.sourceAddress != dev2->GetMac()->GetExtendedAddress()) continue;
//         NS_LOG_UNCOND("----Flow ID:" <<iter->first);
//         NS_LOG_UNCOND("Src Addr" <<t.sourceAddress << "Dst Addr "<< t.destinationAddress);
//         NS_LOG_UNCOND("Sent Packets=" <<iter->second.txPackets);
//         NS_LOG_UNCOND("Received Packets =" <<iter->second.rxPackets);
//         NS_LOG_UNCOND("Lost Packets =" <<iter->second.txPackets-iter->second.rxPackets);
//         NS_LOG_UNCOND("Packet delivery ratio =" <<(double)iter->second.rxPackets*100/iter->second.txPackets << "%");
//         NS_LOG_UNCOND("Packet loss ratio =" << (double)(iter->second.txPackets-iter->second.rxPackets)*100/iter->second.txPackets << "%");
//         NS_LOG_UNCOND("Delay =" <<iter->second.delaySum);
//         NS_LOG_UNCOND("Jitter =" <<iter->second.jitterSum);
//         NS_LOG_UNCOND("Throughput =" <<iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024<<"Kbps\n\n\n");

//         SentPackets = SentPackets +(iter->second.txPackets);
//         ReceivedPackets = ReceivedPackets + (iter->second.rxPackets);
//         LostPackets = LostPackets + (iter->second.txPackets-iter->second.rxPackets);
//         AvgThroughput = AvgThroughput + (iter->second.rxBytes * 8.0/(iter->second.timeLastRxPacket.GetSeconds()-iter->second.timeFirstTxPacket.GetSeconds())/1024);
//         Delay = Delay + (iter->second.delaySum);
//         Jitter = Jitter + (iter->second.jitterSum);

//         j = j + 1;

// 		}

// 		AvgThroughput = AvgThroughput/j;
// 		NS_LOG_UNCOND("--------Total Results of the simulation----------"<<std::endl);
// 		NS_LOG_UNCOND("Total sent packets  =" << SentPackets);
// 		NS_LOG_UNCOND("Total Received Packets =" << ReceivedPackets);
// 		NS_LOG_UNCOND("Total Lost Packets =" << LostPackets);
// 		NS_LOG_UNCOND("Packet Loss ratio =" << ((double)(LostPackets*100)/SentPackets)<< "%");
// 		NS_LOG_UNCOND("Packet delivery ratio =" << ((double)(ReceivedPackets*100)/SentPackets)<< "%");
// 		NS_LOG_UNCOND("Average Throughput =" << AvgThroughput<< "Kbps");
// 		NS_LOG_UNCOND("End to End Delay =" << Delay);
// 		NS_LOG_UNCOND("End to End Jitter delay =" << Jitter); 
// 		NS_LOG_UNCOND("Total Flod id " << j);
// 		monitor->SerializeToXmlFile("manet-routing.xml", true, true );
// 	}

//   Simulator::Destroy ();
//     openGymInterface->NotifySimulationEnd();
//     openGymInterface1->NotifySimulationEnd();
//     return 0;
//   return 0;

// }


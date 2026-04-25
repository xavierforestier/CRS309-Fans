# CRS309-Fans
Hack to add fans to CRS309-1G-8S+

## Mikrotik CRS309-1G-8S+ and 10GB SFP+ RJ45
Mikrotik switch is a 10gb fanless switch, using RJ45 SFP+ coppers convertor (in Mikrotik S+RJ10) causes heating issues.
Mikrotik documented the issueand simply recomment to leave a space https://help.mikrotik.com/docs/spaces/ROS/pages/240156916/S+RJ10+general+guidance

### Symptoms
Tons of logs in router OS : 
```
sfp-sfpplus1 high temperature warning! Please, improve module cooling/placement or use different type of modules for this solution
```
Temperature visible in interface > Ethernet is around 80°C in idle.
SFP+ disconnects regulary for fews minutes

## The Hack
CRS-309 have enough some space for 20-15mmm thick FANs on top on SPF, the purpose is to add :
- 2 x 60mm fans in front of SFP+
- Change original power plug for a 12V model (CRS309 allows 12v - 57v CD input)
- Add an ESP32 and a L293D to pilot FAN speed in PWN
- Add a 12V to 5V DC-DC adpater to power ESP32
- Build a custom PCB
- Code software :
  - Set fans speed to 100%
  - Loop until WIFI is connected
  - Expose a HTTP REST web server :
     - Report fan speed via GET event
     - Handle FAN speed changes via POST event

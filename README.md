# CRS309-1G-8S+ Fan Mods
Hack to add fans to CRS309-1G-8S+

## Mikrotik CRS309-1G-8S+ and 10GB SFP+ RJ45
Mikrotik switch is a 10gb fanless switch, using RJ45 SFP+ coppers convertor (in Mikrotik S+RJ10) causes heating issues.
Mikrotik documented the issue and simply recommend to leave a space https://help.mikrotik.com/docs/spaces/ROS/pages/240156916/S+RJ10+general+guidance

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

BOM : (you can find in Amazon or AliExpress too)
- FAN :
  - 2 x [SUN MF60151V3-1](https://www.reichelt.com/de/en/shop/product/axial_fan_60x60x15mm_12v_29_9m_h_21_3dba-260676)
  - 2 x [DELOCK 67305](https://www.reichelt.com/de/en/shop/product/fan_grill_60_x_60_mm_silver-410074)
- PCB :
  - 1 x : [Breadboard](https://www.reichelt.com/de/en/shop/product/breadboard_laminated_paper_200x100_mm-8273) (100x40mm would be enough)
- Power :
  - 1 x [12V PSU](https://www.reichelt.com/de/en/shop/product/desktop_power_supply_42_w_12_v_3_5_a-215630)
  - 1 x [DC-DC volatge regulator](https://www.reichelt.com/de/en/shop/product/developer_boards_-_voltage_regulator_dc_dc_converter_lm2596-282578)
- Electronic :
  - 1 x [XIAO ESP32S3](https://www.reichelt.com/de/en/shop/product/xiao_esp32s3_dual-core_wifi_bt5_0_with_header-406872) (less powerfull would been enough)
  - 1 x L293D (reused from an arduino kit) 

# Solar Energy Storage

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://www.gnu.org/licenses/mit)
![version](https://img.shields.io/badge/version-1.1.0-blue)

:star: Star us on GitHub — it helps!

Stores the excess solar energy produced during the day to release it at night.<br><br>
This project was done on an Enphase self-consumption solar production facility.<br>
:warning: __It has many electrical hazards. It is best to use an approved energy storage system sold by your solar production system manufacturer.__<br>
This project was done just for the fun of doing it yourself
## Specifications

### System flowchart

```mermaid
flowchart LR;
   ac_in((AC)) --- 5V_power[Permanent\nPower Supply]
   ac_in --- 30V_power[Charge\nPower supply]
   5V_power --- Controller
   30V_power --- Charger
   Charger --- bat[(Battery)]
   bat --- boost[Boost\nCurrent Limiter]
   boost --- inv[Micro-Inverter]
   inv --- ac_out((AC))
```

### Main features
- (main_001) Retrieves continuous production and overproduction informations. (measurement or reading)
- (main_002) Charge one or more batteries with the excess power production.
- (main_003) Releases energy stored on the home grid when solar production is inactive.
- (main_004) Maintain batteries in good condition to preserve their lifespan.
- (main_005) Do not release more stored energy than necessary for home consumption

### Power management
- (power_001) Retrieving data from Envoy gateway every second

Production and overproduction can be measured with AC current measurement clamps.<br>
However for this project, the installation already includes an Enphase Envoy-s Metered gateway. Thus, the energy information can be read on the Ethernet connection.<br>
The Envoy gateway can return a JSON containing lots of information. Simply interrogate in http the gateway on its IP address (example: http://xxx.xxx.xxx.xxx/production.json)

- (power_002) Power start based on overproduction information, battery status and time.

```mermaid
flowchart LR;
    Start --> id1a{Bat. charge\ntoo low};
    id1a -- Yes --> id3
    id1a -- No --> id1b{overproduction\n> 50W\nsince 1 min};
    id1b -- Yes --> id2{Bat. accept\ncharge ?};
    id1b -- No --> id_off;
    id2 -- Yes --> id3{Time Parameters\nControl};
    id2 -- No --> id_off;
    id3 -- OK --> id_on;
    id3 -- NOK --> id_off;
    id_off(Power OFF) --> End;
    id_on(Power ON) --> End;
```
- (power_003) Adjustment of the power delivered to the micro-inverter according to grid consumption and the battery state

```mermaid
flowchart LR;
    Start --> id1{Solar production\n< 10W\nsince 1 min};
    id1 -- Yes --> id2{Sufficient\nbattery charge ?};
    id1 -- No --> id_off;
    id2 -- Yes --> rate[[Power calculation]];
    id2 -- No --> id_off;
    rate --> id_on;
    id_off(Inverter OFF) --> End;
    id_on(Inverter ON) --> End;
```

- (power_004) Power supply status monitoring

```mermaid
flowchart TD;
   monitoring((Monitoring));
   30V_power[Charge\nPower supply];
   charger[Charger];
   30V_power -. temperature .- monitoring
   30V_power -. voltage .- monitoring
   30V_power -. current .- monitoring
   charger -. voltage .- monitoring
   charger -. current .- monitoring
   bat[Battery];
   boost[Boost];
   monitoring -. temperature .- bat
   monitoring -. voltage .- bat
   monitoring -. temperature .- boost
   monitoring -. voltage .- boost
```

- (power_005) Activation/Deactivation of elements for safety and energy saving

The following items can be disabled and disconnected:<br>
Charge power supply<br>
Charger<br>
Boost Current Limiter


### Battery management
- (battery_001) Adapts the charging current according to the available overproduction power.
- (battery_002) Use of charging cycles according to the type of batteries. (Lead-acid, AGM, Li-ION ...)
- (battery_003) Battery Charge and Cycle Rate Calculation
- (battery_004) Discharge Rate Limiting and Deep Discharge Protection


## Hardware

To avoid overconsumed goods, a large part of the Hardware elements were chosen with what I had available. Some choices could be more judicious in performance. However I try to use and reuse as many things as possible without having to buy new.

### Power supply

ATX power supply 550W + Boost module 400W

### Boost Current Limiter / Micro Inverter

Enphase IQ7+

### Controller

Arduino MEGA

## License

MIT License
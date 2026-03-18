# TrueNas Hardware unit hang

```sh
$ dmesg
[137484.902391] e1000e 0000:00:1f.6 eno1: Detected Hardware Unit Hang:
                  TDH                  <ea>
                  TDT                  <50>
                  next_to_use          <50>
                  next_to_clean        <e9>
                buffer_info[next_to_clean]:
                  time_stamp           <102029b2a>
                  next_to_watch        <ea>
                  jiffies              <1020b5090>
                  next_to_watch.status <0>
                MAC Status             <80083>
                PHY Status             <796d>
                PHY 1000BASE-T Status  <3800>
                PHY Extended Status    <3000>
                PCI Status             <10>
```

Fixes (unclear exactly what's the correct setting).
```sh
ethtool -K eno1 gso off gro off tso off tx off rx off rxvlan off txvlan off sg off
ethtool -K eno1 gso off gro off tso off tx off rx off rxvlan off txvlan off sg off

ethtool -K eno1 gso off gro on tso off tx on rx on rxvlan on txvlan on sg on

ethtool -K eth0 tx off rx off

ethtool -a eth0

ethtool --reset <interface>
````

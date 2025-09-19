# ns3-o-ran-e2 aka ns-O-RAN

================================

This ns-3 module enables the support for running multiple terminations of an O-RAN-compliant E2 interface inside the simulation process.
This module has been developed by a team at the [Institute for the Wireless Internet of Things (WIoT)](https://wiot.northeastern.edu) at Northeastern University, in collaboration with Sapienza University of Rome, the University of Padova and with support from Mavenir.

## How to use

This module can be used with an extension of the [ns3-mmWave module](https://github.com/wineslab/ns-o-ran-ns3-mmwave).
This repository must be cloned in the `contrib` folder.
Moreover, our custom version of the [e2sim library](https://github.com/wineslab/o-ran-e2sim) must be installed.
Please refer to this [quick start guide](https://openrangym.com/tutorials/ns-o-ran) that presents a tutorial to bridge ns-O-RAN and Colosseum RIC (i.e., OSC RIC bronze reduced) ns-O-RAN.

Additional material:

- Framework presentation https://openrangym.com/ran-frameworks/ns-o-ran 
- Tutorial OSC RIC version E ns-O-RAN connection  https://www.nsnam.org/tutorials/consortium23/oran-tutorial-slides-wns3-2023.pdf 
- Recording of the tutorial OSC RIC version E done at the WNS3 2023 https://vimeo.com/867704832 
- xApp repositories working with ns-O-RAN:
  - https://github.com/wineslab/ns-o-ran-scp-ric-app-kpimon 
  - https://github.com/wineslab/ns-o-ran-xapp-rc 
- Gymnasium Environment wrapper for ns-O-RAN https://github.com/wineslab/ns-o-ran-gym-environment

We welcome contributions through pull requests. Please contact the authors to submit your contribution.

## References

More information can be found in the technical paper:

> A. Lacava, M. Bordin, M. Polese, R. Sivaraj, T. Zugno, F. Cuomo, and T. Melodia. "ns-O-RAN: Simulating O-RAN 5G Systems in ns-3", Proceedings of the 2023 Workshop on ns-3 (2023), [DOI:10.1145/3592149.3592161](https://dl.acm.org/doi/abs/10.1145/3592149.3592161)

If you use the scenario-one.cc or the traffic steering implementation please cite:

>A. Lacava, M. Polese, R. Sivaraj, R. Soundrarajan, B. Bhati, T. Singh, T. Zugno, F. Cuomo, and T. Melodia. "Programmable and Customized Intelligence for Traffic Steering in 5G Networks Using Open RAN Architectures", IEEE Transactions on Mobile Computing (2024), [DOI:10.1109/TMC.2023.3266642](https://doi.org/10.1109/TMC.2023.3266642) [pdf](https://ieeexplore.ieee.org/document/10102369) [bibtex](https://ece.northeastern.edu/wineslab/wines_bibtex/andrea/LacavaAMC22.txt)

## Authors

The ns3-o-ran-e2 module is the result of the development effort carried out by different people. The main contributors are:

- Andrea Lacava, Northeastern University and Sapienza University of Rome
- Michele Polese, Northeastern University
- Tommaso Zugno, University of Padova
- Rajarajan Sivaraj and team, Mavenir

## Acknowledgements

This work was partially supported by Mavenir, by Sapienza, University of Rome under Grant AR1221816B3DC365 and by the U.S. National Science Foundation under Grants CNS-1923789 and CNS-2112471.

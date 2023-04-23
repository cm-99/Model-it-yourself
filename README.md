# Model-it-yourself

I am pleased to present a project that for the moment has nothing to do with its great name. The target application is to allow data processing, modelling, verification of results and simulation using the model created. For the moment, data loading, pre-processing and visualisation are available (to a limited extent).  
This was my first experience with building an application from scratch in MVC architecture. I hope that the base I have created will allow for rapid development of further parts of the application and I will come by here often to check off further targets.

## Features
- **Csv files import:** ...which accepts different delimiters
- **Built-in data editor:** You can edit imported data by selecting rows or columns to remove, or use range-based data removal tool.
- **General dataset attributes and signals statistics:** Under a separate tab you can view informations about imported data. This includes general attributes such as numbers of signals, rows and calculated sampling time (if timestamps column was provided). Additionaly signals details are provided - min and max values for each, ability to set their names and units. This will be useful in the future.
- **Data charts:** ...with the ability to chose which signals should be displayed and automatical adjustment of the values ranges.

For those who would rather see it, wait a moment, gif is loading:
![alt text](demos/MIY-features.gif)

## Built with
Qt 5.15.

## License
This project is licensed under GPLv3 license.

## Further development and milestones
- **General objectives/architecture:**
  - cleaning up all TODO comments. Either by implementing what is stated in them or removing what does not seem necessary any more.
  - refactoring code in essentials and converting interfaces/implementations to plugins
  - extending logging functionality to include creation of xml files which would depict call sequences and could be used to easily create sequence diagrams. 
   In order not to introduce a bottleneck, xml should propably be created on program exit from already existing txt logs.
  - creating documentation and filling in missing tests.
- **Data modelling:**
  - creating all the necessary GUI, model classes, identification using LSTM... and much more. 
    There is little to no point in going into lenghty descriptions before completing the previous part and this time preparing the structure before implementation.

import React from "react";
import "../css/LandingPage.css";
import ArenaMap from "./ArenaMap";
import { getMapDescriptor } from '../simulator/mapDescriptor';
 

class LandingPage extends React.Component {
  constructor(props){
    super(props);
    this.state = {
      descriptor1: "",
      descriptor2: "",
      arena: {}
    }
  }

  setMapDescriptor = () => {
    const arenaMode = this.refs.map_descriptor.value;
    this.setState({ 
      arena: {
        Height: 20,
        Width: 15,
        Arena: getMapDescriptor(arenaMode.toUpperCase())
      }
    });
  }

  render() {
    return (
      <body>
        <div className="wrapper">
          <div className="flex-nav">
            <ul>
              <button type="button">Simulation Mode</button>
              <button type="button">Actual Mode</button>
            </ul>
          </div>
        </div>
        <div className="spacing"></div>

        <div className="Map-Wrapper">
          <ArenaMap arena={this.state.arena}/>
        </div>
        <div className="spacing"></div>
        <div className="Inputs-Wrapper">
          <div className="spacing_2"></div>
          <input ref="map_descriptor"
            className="MapDescriptor_text"
            type="text"
            placeholder="Map Descriptor"
          ></input>
          <input onClick={this.setMapDescriptor}
            className="MapDescriptor_button"
            type="submit"
            value="load"
          ></input>
          <div className="spacing_2"></div>
        </div>
        <div className="spacing"></div>
        <div className="Overall-Inputs-Wrapper">
          <div className="Inputs-1">
            <div>
              Step:{" "}
              <input type="text" placeholder="Step in milliseconds"></input>ms
            </div>
            <div>
              TimeOut:
              <input type="text" placeholder="TimeOut"></input>s
            </div>
            <div>
              Coverage:
              <input type="text" placeholder="Coverage"></input>s
            </div>
            <div>
              Start X-Cord:
              <input type="text" placeholder="Start X-Cord"></input>s
            </div>
            <div>
              Start Y-Cord:
              <input type="text" placeholder="Start Y-Cord:"></input>s
            </div>
            <div>
              Start Direction:
              <select name="Direction-Select" id="Direction">
                <option value="Up">Up</option>
                <option value="Up">Down</option>
                <option value="Up">Left</option>
                <option value="Up">Right</option>
              </select>
            </div>
            <div>
              Goal X-Cord:
              <input type="text" placeholder="Goal X-Cord"></input>
            </div>
            <div>
              Goal Y-Cord:
              <input type="text" placeholder="Goal Y-Cord"></input>
            </div>
          </div>
          <div className="Inputs-space"></div>
          <div className="Inputs-2">
            <div>
              Map Descriptor (1):
              <input type="text" placeholder="" value={this.state.descriptor1} key={this.state.descriptor1}></input>
            </div>
            <div>
              Map Descriptor (2):
              <input type="text" placeholder="" value={this.state.descriptor2} key={this.state.descriptor2}></input>
            </div>
          </div>
          <div className="Inputs-space"></div>
          <div className="Inputs-3">
            <div>
              Checkpoint X-Coord:
              <input type="text" placeholder=""></input>
            </div>
            <div>
              Checkpoint Y-Coord:
              <input type="text" placeholder=""></input>
            </div>
          </div>
        </div>
        <div className="spacing"></div>
        <div className="footer-buttons">
            <button type="button">Exploration</button>
            <div className="button-space"></div>
            <button type="button">Send MDF to Algorithm</button>
            <div className="button-space"></div>
            <button type="button">Fastest Path</button>
        </div>
      </body>
    );
  }
}

export default LandingPage;

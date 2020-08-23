import React from "react";
import "../css/LandingPage.css";
import ArenaMap from "./ArenaMap";

class LandingPage extends React.Component {
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
          <ArenaMap />
        </div>
        <div className="spacing"></div>
        <div className="Inputs-Wrapper">
          <div className="spacing_2"></div>
          <input
            className="MapDescriptor_text"
            type="text"
            placeholder="Map Descriptor"
          ></input>
          <input
            className="MapDescriptor_button"
            type="submit"
            value="load"
          ></input>
          <div className="spacing_2"></div>
        </div>
        <div className="spacing"></div>
        <div className="Overall-Inputs-Wrapper">
          <div className="Inputs-1">
            <a>
              Step:{" "}
              <input type="text" placeholder="Step in milliseconds"></input>ms
            </a>
            <a>
              TimeOut:
              <input type="text" placeholder="TimeOut"></input>s
            </a>
            <a>
              Coverage:
              <input type="text" placeholder="Coverage"></input>s
            </a>
            <a>
              Start X-Cord:
              <input type="text" placeholder="Start X-Cord"></input>s
            </a>
            <a>
              Start Y-Cord:
              <input type="text" placeholder="Start Y-Cord:"></input>s
            </a>
            <a>
              Start Direction:
              <select name="Direction-Select" id="Direction">
                <option value="Up">Up</option>
                <option value="Up">Down</option>
                <option value="Up">Left</option>
                <option value="Up">Right</option>
              </select>
            </a>
            <a>
              Goal X-Cord:
              <input type="text" placeholder="Goal X-Cord"></input>
            </a>
            <a>
              Goal Y-Cord:
              <input type="text" placeholder="Goal Y-Cord"></input>
            </a>
          </div>
          <div className="Inputs-space"></div>
          <div className="Inputs-2">
            <a>
              Map Descriptor (1):
              <input type="text" placeholder=""></input>
            </a>
            <a>
              Map Descriptor (2):
              <input type="text" placeholder=""></input>
            </a>
          </div>
          <div className="Inputs-space"></div>
          <div className="Inputs-3">
          <a>
              Checkpoint X-Coord:
              <input type="text" placeholder=""></input>
            </a>
            <a>
              Checkpoint Y-Coord:
              <input type="text" placeholder=""></input>
            </a>
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

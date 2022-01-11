#include "plugin.hpp"

struct Jigsaw : Module {
   enum ParamIds {
	  ATTACK,
	  SUSTAIN,
	  DECAY,
      MINV,
      MAXV,
      MINV2,
      MAXV2,
      PARAM1,
      PARAM2,
      PARAM3,
      PARAM4,
      KNOB1,
      KNOB2,
      KNOB3,
      KNOB4,
      MULTI,
      DIVI,
      NUM_PARAMS
   };
   enum InputIds {
      V1_INPUT,
      V2_INPUT,
      NUM_INPUTS
   };
   enum OutputIds {
      NOT_OUTPUT,
      SINE_OUTPUT,
      NCOS_OUTPUT,
      COS_OUTPUT,
      V0_OUTPUT,
      NUM_OUTPUTS
   };
   enum LightIds {
      BLINK_LIGHT,
      NUM_LIGHTS
   };

   Jigsaw() {
      config(NUM_PARAMS, NUM_INPUTS,
      NUM_OUTPUTS, NUM_LIGHTS);
      
      configParam(PARAM3, 0.f, 176400.f, 440.f, "Freq");
      configParam(PARAM4, -2.f, 4.f, 0.f, "Pitch");
      configParam(KNOB1, 1.f, 8.f, 2.f, "Mults");

      configParam(PARAM1, 1.f, 420.f, 48.f, "Mul");
      configParam(MINV, -12.f, 12.f, -5.f, "MINV");
      configParam(MINV2, -12.f, 12.f, 5.f, "MINV");

      configParam(PARAM2, 1.f, 420.f, 48.f, "Div");
      configParam(MAXV, -12.f, 12.f, 5.f, "MAXV");
      configParam(MAXV2, -12.f, 12.f, -5.f, "MAXV");

      configParam(MULTI, -12.f, 16.f, 1.f, "Multi");
      configParam(DIVI, 1.f, 16.f, 4.f, "Divi");
	  
      configParam(ATTACK, 0.f, 36.f, 6.f, "Attack");
      configParam(SUSTAIN, 0.f, 36.f, 12.f, "Sustain");
      configParam(DECAY, 0.f, 36.f, 6.f, "Release");

      configParam(KNOB4, -48.f, 48.f, -9.f, "Tone");
      configParam(KNOB2, 1.f, 48.f, 12.f, "Divs");
      configParam(KNOB3, 1.f, 48.f, 1.f, "Parts");
	  /*
	  configInput(V1_INPUT, "V1");
	  configInput(V2_INPUT, "V2");
	  
	  configOutput(V0_OUTPUT, "V0");
	  configOutput(NOT_OUTPUT, "NSIN");
	  configOutput(SINE_OUTPUT, "SIN");
	  configOutput(NCOS_OUTPUT, "NCOS");
	  configOutput(COS_OUTPUT, "COS"); */
   }

    void process(const ProcessArgs &args) override {
    	int channels = std::max({1, inputs[V1_INPUT].getChannels(), inputs[V2_INPUT].getChannels()});

        outputs[NOT_OUTPUT].setChannels(channels);
        outputs[SINE_OUTPUT].setChannels(channels);
        outputs[NCOS_OUTPUT].setChannels(channels);
        outputs[COS_OUTPUT].setChannels(channels);
        outputs[V0_OUTPUT].setChannels(channels);
        
		float blink,sine,cos;
		float total = params[ATTACK].getValue()+params[SUSTAIN].getValue()+params[DECAY].getValue();
			
		if (total == 0) {
			lights[BLINK_LIGHT].setBrightness(0.5);
			for (int c = 0; c < channels; c++) {
				outputs[NOT_OUTPUT].setVoltage(0, c);
				outputs[SINE_OUTPUT].setVoltage(0, c);
				outputs[NCOS_OUTPUT].setVoltage(0, c);
				outputs[COS_OUTPUT].setVoltage(0, c);
			}	return;
		}
		
		for (int c = 0; c < channels; c++) {
			
	        float pitch = params[PARAM4].getValue();
	        pitch += inputs[V1_INPUT].getVoltage(c)+inputs[V2_INPUT].getVoltage(c)/params[KNOB3].getValue();
	        float freq = params[PARAM3].getValue()*params[PARAM2].getValue()/params[PARAM1].getValue() * std::pow(params[KNOB1].getValue(), pitch+params[KNOB4].getValue()/params[KNOB2].getValue());
	
	        // Accumulate the phase
	        phase[c] += freq * args.sampleTime;
	        while (phase[c] > 0.5f)
	            phase[c] -= 1.f;
	
	        phase2[c] = phase[c]+params[MULTI].getValue()/params[DIVI].getValue();
	        while (std::abs(phase2[c]) > 0.5f)
	            phase2[c] -= phase2[c]/std::abs(phase2[c]);
	
			float V0 = inputs[V1_INPUT].getVoltage(c)+inputs[V2_INPUT].getVoltage(c)/params[KNOB3].getValue()+params[KNOB4].getValue()/params[KNOB2].getValue();
			outputs[V0_OUTPUT].setVoltage(clamp(V0, -12.f, 12.f), c);
			
			if (std::abs(phase[c]) < params[ATTACK].getValue()/total/2) {
				sine = phase[c]*total*2/params[ATTACK].getValue();
			} else if (std::abs(phase[c]) <= params[ATTACK].getValue()/total/2+params[SUSTAIN].getValue()/total/2) {
				sine = phase[c]/std::abs(phase[c]);
			} else { sine = phase[c]/std::abs(phase[c])/2-phase[c];
				sine = sine*total*2/params[DECAY].getValue();
			}
			
			if (std::abs(phase2[c]) < params[ATTACK].getValue()/total/2) {
				cos = phase2[c]*total*2/params[ATTACK].getValue();
			} else if (std::abs(phase2[c]) <= params[ATTACK].getValue()/total/2+params[SUSTAIN].getValue()/total/2) {
				cos = phase2[c]/std::abs(phase2[c]);
			} else { cos = phase2[c]/std::abs(phase2[c])/2-phase2[c];
				cos = cos*total*2/params[DECAY].getValue();
			}
	
	        // Audio signals are typically +/-5V
	        // https://vcvrack.com/manual/VoltageStandards.html
	        outputs[NOT_OUTPUT].setVoltage((params[MAXV2].getValue()-params[MINV2].getValue()) * (sine+1)/2 + params[MINV2].getValue(), c);
	        outputs[SINE_OUTPUT].setVoltage((params[MAXV].getValue()-params[MINV].getValue()) * (sine+1)/2 + params[MINV].getValue(), c);
	        outputs[NCOS_OUTPUT].setVoltage((params[MAXV2].getValue()-params[MINV2].getValue()) * (cos+1)/2 + params[MINV2].getValue(), c);
	        outputs[COS_OUTPUT].setVoltage((params[MAXV].getValue()-params[MINV].getValue()) * (cos+1)/2 + params[MINV].getValue(), c);
		}
		
	    // Blink light at 1Hz
	    bPhase += params[PARAM2].getValue()/params[PARAM1].getValue()*args.sampleTime;
	    while (bPhase > 0.5) bPhase -= 1.f;
	
		if (std::abs(bPhase) < params[ATTACK].getValue()/total/2) {
			blink = bPhase*total*2/params[ATTACK].getValue();
		} else if (std::abs(bPhase) <= params[ATTACK].getValue()/total/2+params[SUSTAIN].getValue()/total/2) {
			blink = bPhase/std::abs(bPhase);
		} else { blink = bPhase/std::abs(bPhase)/2-bPhase;
			blink = blink*total*2/params[DECAY].getValue();
		}
		
        lights[BLINK_LIGHT].setBrightness(blink/2+1/2);
    }

   float phase[16] = { 0.f };
   float phase2[16] = { 0.f };
   float bPhase = 0.f;
};


struct JigsawWidget : ModuleWidget {
   JigsawWidget(Jigsaw* module) {
      setModule(module);
      setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Jigsaw.svg")));

      addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
      addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
      addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
      addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

      addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(15.24*2, 6.425)), module, Jigsaw::BLINK_LIGHT));

      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*1+6.425)), module, Jigsaw::PARAM3));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*1+6.425)), module, Jigsaw::PARAM4));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*1+6.425)), module, Jigsaw::KNOB1));


      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*2+6.425)), module, Jigsaw::PARAM1));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*2+6.425)), module, Jigsaw::MINV));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*2+6.425)), module, Jigsaw::MINV2));


      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*3+6.425)), module, Jigsaw::PARAM2));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*3+6.425)), module, Jigsaw::MAXV));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*3+6.425)), module, Jigsaw::MAXV2));


      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*4+6.425)), module, Jigsaw::MULTI));
      addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*4+6.425)), module, Jigsaw::SINE_OUTPUT));
      addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*4+6.425)), module, Jigsaw::NOT_OUTPUT));

	  
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*5+6.425)), module, Jigsaw::DIVI));
      addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*5+6.425)), module, Jigsaw::COS_OUTPUT));
      addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*5+6.425)), module, Jigsaw::NCOS_OUTPUT));

	  
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*6+6.425)), module, Jigsaw::ATTACK));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*6+6.425)), module, Jigsaw::SUSTAIN));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*6+6.425)), module, Jigsaw::DECAY));
	  

      addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*7+6.425)), module, Jigsaw::V1_INPUT));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*7+6.425)), module, Jigsaw::KNOB2));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*7+6.425)), module, Jigsaw::KNOB4));


      addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*8+6.425)), module, Jigsaw::V2_INPUT));
      addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*8+6.425)), module, Jigsaw::KNOB3));
      addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*8+6.425)), module, Jigsaw::V0_OUTPUT));

   }

};


Model* modelJigsaw = createModel<Jigsaw, JigsawWidget>("Jigsaw");

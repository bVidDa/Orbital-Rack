#include "plugin.hpp"

struct Turing : Module {
	enum ParamIds {
		ENUMS(PARAM4, 6),
		ENUMS(PARAM5, 6),
		ENUMS(PARAM6, 4),
		NUM_PARAMS
	};
	enum InputIds {
		ENUMS(CV_INPUT, 12),
		ENUMS(IN_INPUT, 2),
		NUM_INPUTS
	};
	enum OutputIds {
		ENUMS(OUTPUT3, 3),
		ENUMS(OUTPUTS, 9),
		ENUMS(OUTPUT2, 9),
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	Turing() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
      configParam(PARAM6+0, -60.f, 360.f, 4.f, "Mul");
      configParam(PARAM6+1, 1.f, 420.f, 4.f, "Div");
      configParam(PARAM6+2, -60.f, 360.f, 4.f, "Mul");
      configParam(PARAM6+3, 1.f, 420.f, 4.f, "Div");
	  
	  configInput(IN_INPUT+0, "IN");
	  configInput(IN_INPUT+1, "IN");
	  
	  configOutput(OUTPUTS+0, "1");
	  configOutput(OUTPUTS+1, "sqr(2)");
	  configOutput(OUTPUTS+2, "(Sqr(5)+1)/2");
	  configOutput(OUTPUTS+3, "PI");
	  configOutput(OUTPUTS+4, "-1");
	  configOutput(OUTPUTS+5, "1/sqr(2)");
	  configOutput(OUTPUTS+6, "2/(Sqr(5)+1)");
	  configOutput(OUTPUTS+7, "E");
	  configOutput(OUTPUTS+8, "OUT");
	  
	  configOutput(OUTPUT2+0, "1");
	  configOutput(OUTPUT2+1, "sqr(2)");
	  configOutput(OUTPUT2+2, "(Sqr(5)+1)/2");
	  configOutput(OUTPUT2+3, "PI");
	  configOutput(OUTPUT2+4, "-1");
	  configOutput(OUTPUT2+5, "1/sqr(2)");
	  configOutput(OUTPUT2+6, "2/(Sqr(5)+1)");
	  configOutput(OUTPUT2+7, "E");
	  configOutput(OUTPUT2+8, "OUT");
	}

	void process(const ProcessArgs& args) override {
    	int channels = std::max({1, inputs[IN_INPUT+0].getChannels()});
    	int channel2 = std::max({1, inputs[IN_INPUT+1].getChannels()});

        outputs[OUTPUTS+0].setChannels(channels);
        outputs[OUTPUTS+1].setChannels(channels);
        outputs[OUTPUTS+2].setChannels(channels);
        outputs[OUTPUTS+3].setChannels(channels);
        outputs[OUTPUTS+4].setChannels(channels);
        outputs[OUTPUTS+5].setChannels(channels);
        outputs[OUTPUTS+6].setChannels(channels);
        outputs[OUTPUTS+7].setChannels(channels);
        outputs[OUTPUTS+8].setChannels(channels);
        
        outputs[OUTPUT2+0].setChannels(channel2);
        outputs[OUTPUT2+1].setChannels(channel2);
        outputs[OUTPUT2+2].setChannels(channel2);
        outputs[OUTPUT2+3].setChannels(channel2);
        outputs[OUTPUT2+4].setChannels(channel2);
        outputs[OUTPUT2+5].setChannels(channel2);
        outputs[OUTPUT2+6].setChannels(channel2);
        outputs[OUTPUT2+7].setChannels(channel2);
        outputs[OUTPUT2+8].setChannels(channel2);
		
		float val[8];
		val[0] = vals[0]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[1] = vals[1]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[2] = vals[2]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[3] = vals[3]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[4] = vals[4]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[5] = vals[5]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[6] = vals[6]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		val[7] = vals[7]*params[PARAM6+0].getValue()/params[PARAM6+1].getValue();
		
		for (int c = 0; c < channels; c++) {
			for (int i = 0; i < 8; i++) {
				outputs[OUTPUTS+i].setVoltage(val[i],c);
			}
			outputs[OUTPUTS+8].setVoltage(inputs[IN_INPUT+0].getVoltage(c)*params[PARAM6+0].getValue()/params[PARAM6+1].getValue(),c);
		}
		
		val[0] = vals[0]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[1] = vals[1]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[2] = vals[2]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[3] = vals[3]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[4] = vals[4]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[5] = vals[5]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[6] = vals[6]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
		val[7] = vals[7]*params[PARAM6+2].getValue()/params[PARAM6+3].getValue();
	
		for (int c = 0; c < channel2; c++) {
			for (int i = 0; i < 8; i++) {
				outputs[OUTPUT2+i].setVoltage(val[i],c);
			}
			outputs[OUTPUT2+8].setVoltage(inputs[IN_INPUT+1].getVoltage(c)*params[PARAM6+2].getValue()/params[PARAM6+3].getValue(),c);
		}
	}
	
	float vals[8] = { 1.f, std::sqrt(2.f), std::sqrt(5.f)/2.f+0.5f, M_PI, -1.f, 1.f/std::sqrt(2.f), std::sqrt(5.f)/2.f-0.5f, std::exp(1.f) };
};


struct TuringWidget : ModuleWidget {
	TuringWidget(Turing* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Turing.svg")));

      addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
      addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
      addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
      addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*5+6.425)), module, Turing::PARAM4 + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*5+6.425)), module, Turing::PARAM4 + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*5+6.425)), module, Turing::PARAM4 + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*4, 12.85*5+6.425)), module, Turing::PARAM4 + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*5, 12.85*5+6.425)), module, Turing::PARAM4 + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*6, 12.85*5+6.425)), module, Turing::PARAM4 + 5));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*6+6.425)), module, Turing::PARAM5 + 0));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*2, 12.85*6+6.425)), module, Turing::PARAM5 + 1));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*3, 12.85*6+6.425)), module, Turing::PARAM5 + 2));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*4, 12.85*6+6.425)), module, Turing::PARAM5 + 3));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*5, 12.85*6+6.425)), module, Turing::PARAM5 + 4));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*6, 12.85*6+6.425)), module, Turing::PARAM5 + 5));

		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*1+6.425)), module, Turing::PARAM6 + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*1+6.425)), module, Turing::OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*1+6.425)), module, Turing::OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*1+6.425)), module, Turing::OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*5, 12.85*1+6.425)), module, Turing::OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*6, 12.85*1+6.425)), module, Turing::OUTPUTS + 8));
		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*2+6.425)), module, Turing::PARAM6 + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*2+6.425)), module, Turing::OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*2+6.425)), module, Turing::OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*2+6.425)), module, Turing::OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*5, 12.85*2+6.425)), module, Turing::OUTPUTS + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*6, 12.85*2+6.425)), module, Turing::IN_INPUT + 0));
		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*3+6.425)), module, Turing::PARAM6 + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*3+6.425)), module, Turing::OUTPUT2 + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*3+6.425)), module, Turing::OUTPUT2 + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*3+6.425)), module, Turing::OUTPUT2 + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*5, 12.85*3+6.425)), module, Turing::OUTPUT2 + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*6, 12.85*3+6.425)), module, Turing::OUTPUT2 + 8));
		
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24*1, 12.85*4+6.425)), module, Turing::PARAM6 + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*4+6.425)), module, Turing::OUTPUT2 + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*4+6.425)), module, Turing::OUTPUT2 + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*4+6.425)), module, Turing::OUTPUT2 + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*5, 12.85*4+6.425)), module, Turing::OUTPUT2 + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*6, 12.85*4+6.425)), module, Turing::IN_INPUT + 1));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*7+6.425)), module, Turing::CV_INPUT + 0));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*7+6.425)), module, Turing::CV_INPUT + 1));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*7+6.425)), module, Turing::CV_INPUT + 2));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*7+6.425)), module, Turing::CV_INPUT + 3));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*5, 12.85*7+6.425)), module, Turing::CV_INPUT + 4));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*6, 12.85*7+6.425)), module, Turing::CV_INPUT + 5));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*1, 12.85*8+6.425)), module, Turing::CV_INPUT + 6));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*3, 12.85*8+6.425)), module, Turing::CV_INPUT + 7));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24*5, 12.85*8+6.425)), module, Turing::CV_INPUT + 8));
		
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*2, 12.85*8+6.425)), module, Turing::OUTPUT3 + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*4, 12.85*8+6.425)), module, Turing::OUTPUT3 + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.24*6, 12.85*8+6.425)), module, Turing::OUTPUT3 + 2));
		
		
	}

};

Model* modelTuring = createModel<Turing, TuringWidget>("Turing");
# DAC-SDC2019冠军项目skynet复现
 该项目在2019年的DAC-SDC大赛中取得了冠军，该项目在Ultra96系列FPGA上实现了 0.716 IoU和25.05 FPS，性能优异。本项目将该项目在百度Edgeboard ZU3系列FPGA上进行了复现。  

 # 原项目GitHub链接  
 ```
https://github.com/TomG008/SkyNet
 ```
# 文件说明
vivado：FPGA硬件工程   
HLS：加速IP实现工程  
pynq：上传至开发板的文件  


# 相关教程
请关注公众号：佛系的FPGAer  
![avatar](https://github.com/buaa-zzx/Edgeboard-MPSOC-FPGA/blob/main/Ubuntu%E7%B3%BB%E7%BB%9F%E7%A7%BB%E6%A4%8D/weichart.jpg)

# References
If you find SkyNet useful, please cite the [SkyNet paper](https://arxiv.org/abs/1909.09709):
```
@inproceedings{zhang2020skynet,
  title={{SkyNet}: a hardware-efficient method for object detection and tracking on embedded systems},
  author={Zhang, Xiaofan and Lu, Haoming and Hao, Cong and Li, Jiachen and Cheng, Bowen and Li, Yuhong and Rupnow, Kyle and Xiong, Jinjun and Huang, Thomas and Shi, Honghui and Hwu, Wen-mei and Chen, Deming},
  booktitle={Conference on Machine Learning and Systems (MLSys)},
  year={2020}
}
```
More details regarding the SkyNet design motivations and SkyNet FPGA accelerator design can be found in our [ICML'19 workshop paper](https://arxiv.org/abs/1905.08369) (which won the **Best Poster Award**) and the [DAC'19 paper](https://arxiv.org/abs/1904.04421), respectively.
```
@article{zhang2019bi,
  title={A Bi-Directional Co-Design Approach to Enable Deep Learning on {IoT} Devices},
  author={Zhang, Xiaofan and Hao, Cong and Li, Yuhong and Chen, Yao and Xiong, Jinjun and Hwu, Wen-mei and Chen, Deming},
  journal={arXiv preprint arXiv:1905.08369},
  year={2019}
}
```
```
@inproceedings{hao2019fpga,
  title={{FPGA/DNN} Co-Design: An Efficient Design Methodology for {IoT} Intelligence on the Edge},
  author={Hao, Cong and Zhang, Xiaofan and Li, Yuhong and Huang, Sitao and Xiong, Jinjun and Rupnow, Kyle and Hwu, Wen-mei and Chen, Deming},
  booktitle={Proceedings of the 56th ACM/IEEE Design Automation Conference (DAC)},
  year={2019}
}
```
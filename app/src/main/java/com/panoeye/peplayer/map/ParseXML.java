package com.panoeye.peplayer.map;

import android.util.Log;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

/**
 * Created by Administrator on 2018/3/18.
 * Java——DOM方式生成XML
 * http://blog.csdn.net/u012325167/article/details/50943202
 */

public class ParseXML {
    private static final String TAG = "ParseXML";

    static public void createXMLByDOM(File dest) {
        // 创建DocumentBuilderFactory
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        try {

            // 创建DocumentBuilder
            DocumentBuilder builder = factory.newDocumentBuilder();

            // 创建Document，该对象代表一个XML文件
            Document document = builder.newDocument();

            // 设置XML声明中standalone为yes，即没有dtd和schema作为该XML的说明文档，且不显示该属性
            // document.setXmlStandalone(true);

            // 创建根节点
            Element cameraList = document.createElement("cameraList");

            // 创建子节点，并设置属性
            Element camera = document.createElement("camera");
            camera.setAttribute("id", "1");

            // 为book添加子节点
            Element ip = document.createElement("ip");
            ip.setTextContent("192.168.20.178");
            camera.appendChild(ip);
            Element bin = document.createElement("bin");
            bin.setTextContent("E10010078");
            camera.appendChild(bin);
            Element cameraType = document.createElement("cameraType");
            cameraType.setTextContent("8");
            camera.appendChild(cameraType);
            Element showName = document.createElement("showName");
            showName.setTextContent("E178");
            camera.appendChild(showName);
            // 为根节点添加子节点
            cameraList.appendChild(camera);

            // 将根节点添加到Document下
            document.appendChild(cameraList);

            /*
             * 下面开始实现： 生成XML文件
             */

            // 创建TransformerFactory对象
            TransformerFactory tff = TransformerFactory.newInstance();

            // 创建Transformer对象
            Transformer tf = tff.newTransformer();

            // 设置换行
            tf.setOutputProperty(OutputKeys.INDENT, "yes");
            //设置缩进量
            tf.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "4");

            // 使用Transformer的transform()方法将DOM树转换成XML
            tf.transform(new DOMSource(document), new StreamResult(dest));

        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        } catch (TransformerConfigurationException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (TransformerException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }

    static public void editXMLByDOM(File dest,String cID,String cIP,String cBin,String cType,String cName) {
        // 创建DocumentBuilderFactory
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        try {

            // 创建DocumentBuilder
            DocumentBuilder builder = factory.newDocumentBuilder();

            // 创建Document，该对象代表一个XML文件
            Document document = builder.newDocument();

            // 设置XML声明中standalone为yes，即没有dtd和schema作为该XML的说明文档，且不显示该属性
            // document.setXmlStandalone(true);

            // 创建根节点
            Element cameraList = document.createElement("cameraList");

            // 创建子节点，并设置属性
            Element camera = document.createElement("camera");
            camera.setAttribute("id", cID);

            // 为book添加子节点
            Element ip = document.createElement("ip");
            ip.setTextContent(cIP);
            camera.appendChild(ip);
            Element bin = document.createElement("bin");
            bin.setTextContent(cBin);
            camera.appendChild(bin);
            Element cameraType = document.createElement("cameraType");
            cameraType.setTextContent(cType);
            camera.appendChild(cameraType);
            Element showName = document.createElement("showName");
            showName.setTextContent(cName);
            camera.appendChild(showName);
            // 为根节点添加子节点
            cameraList.appendChild(camera);

            // 将根节点添加到Document下
            document.appendChild(cameraList);

            /*
             * 下面开始实现： 生成XML文件
             */

            // 创建TransformerFactory对象
            TransformerFactory tff = TransformerFactory.newInstance();

            // 创建Transformer对象
            Transformer tf = tff.newTransformer();

            // 设置换行
            tf.setOutputProperty(OutputKeys.INDENT, "yes");
            //设置缩进量
            tf.setOutputProperty("{http://xml.apache.org/xslt}indent-amount", "4");

            // 使用Transformer的transform()方法将DOM树转换成XML
            tf.transform(new DOMSource(document), new StreamResult(dest));

        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        } catch (TransformerConfigurationException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (TransformerException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

    }

    static public ArrayList parseXMLByDOM(File file) {
        ArrayList<Camera> cameras= new ArrayList<Camera>();
        //创建DocumentBuilderFactory的对象
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        try {
            //通过DocumentBuilderFactory创建一个DocumentBuilder对象
            DocumentBuilder builder = factory.newDocumentBuilder();

            //通过DocumentBuilder对象的parse方法解析xml文件,并用Document对象接收
            Document document = builder.parse(file);

            //通过document.getElementsByTagName获取所有名为book的节点的集合
            NodeList bookList = document.getElementsByTagName("camera");

            System.out.println("共有 " + bookList.getLength() + " 个相机");

            //遍历每一本书
            for(int i=0; i<bookList.getLength(); ++i){
                System.out.println("第" + (i+1) + "个相机:");
                Node book = bookList.item(i);

                Camera camera = new Camera();

                //获取book节点的所有属性
                NamedNodeMap attrs = book.getAttributes();
                for(int k=0; k<attrs.getLength(); ++k){
                    Node node = attrs.item(k);
                    System.out.println(node.getNodeName() + " : " + node.getNodeValue());
                    if (node.getNodeName().equals("id")){
                        Log.d(TAG, "parseXMLByDOM: "+ node.getNodeValue());
                        camera.setId(Integer.parseInt(node.getNodeValue()));
                    }
                }

                //获取book节点的所有子节点的 名及值
                NodeList child = book.getChildNodes();
                for(int j=0; j<child.getLength(); ++j){
                    Node node = child.item(j);
                    //过滤掉换行、空格等text类型的节点，只保留带标签的元素节点
                    if(node.getNodeType() == Node.ELEMENT_NODE){
                        //标签之间的内容被认为是该标签的子节点,所以需要使用node.getFirstChild()获取到这个子节点的第一个孩子节点后再获取其值
                        switch(node.getNodeName()){
                            case "ip" :
                                camera.setIP(node.getFirstChild().getNodeValue());
                                break;
                            case "bin" :
                                camera.setBin(node.getFirstChild().getNodeValue());
                                break;
                            case "cameraType":
                                camera.setCameraType(Integer.valueOf(node.getFirstChild().getNodeValue()));
                                break;
                        }

                        //使用getTextContent()方法可获取该标签节点下的所有子节点内容
//                      System.out.println(node.getNodeName() + " : " + node.getTextContent());
                    }
                }
                System.out.println(camera);
                cameras.add(camera);
            }

        } catch (ParserConfigurationException e) {
            e.printStackTrace();
        } catch (SAXException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return cameras;
    }

}

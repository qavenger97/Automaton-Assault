using UnityEngine;
using System.Collections;
using UnityEditor;
using System.Collections.Generic;
using System.Xml;

public class SceneExproter : EditorWindow
{
    [MenuItem("Window/Just Machine Scene Exporter")]
    public static void ShowWindow()
    {
        SceneExproter.GetWindow(typeof(SceneExproter));
    }

    void ExportObject(XmlWriter writer, Transform transform)
    {
        writer.WriteStartElement("Entity");
        {
            writer.WriteAttributeString("name", transform.name);

            writer.WriteStartElement("Transform");
            {
                writer.WriteStartElement("Position");
                {
                    writer.WriteAttributeString("x", transform.localPosition.x.ToString());
                    writer.WriteAttributeString("y", transform.localPosition.y.ToString());
                    writer.WriteAttributeString("z", transform.localPosition.z.ToString());
                }
                writer.WriteEndElement();

                writer.WriteStartElement("Rotation");
                {
                    Vector3 rot = transform.localRotation.eulerAngles;
                    writer.WriteAttributeString("pitch", (Mathf.Deg2Rad * rot.x).ToString());
                    writer.WriteAttributeString("yaw", (Mathf.Deg2Rad * rot.y).ToString());
                    writer.WriteAttributeString("roll", (Mathf.Deg2Rad * rot.z).ToString());
                }
                writer.WriteEndElement();

                writer.WriteStartElement("Scale");
                {
                    writer.WriteAttributeString("x", transform.localScale.x.ToString());
                    writer.WriteAttributeString("y", transform.localScale.y.ToString());
                    writer.WriteAttributeString("z", transform.localScale.z.ToString());
                }
                writer.WriteEndElement();
            }
            writer.WriteEndElement();

            writer.WriteStartElement("Components");
            {
                MeshRenderer renderer = transform.GetComponent<MeshRenderer>();
                MeshFilter meshFilter = transform.GetComponent<MeshFilter>();

                if (renderer && meshFilter)
                {
                    writer.WriteStartElement("MeshRenderer");
                    {
                        writer.WriteStartElement("Mesh");

                        string meshName = transform.GetComponent<MeshFilter>().sharedMesh.name;
                        if (meshName == "Cylinder")
                        {
                            writer.WriteAttributeString("file", "Assets/Mesh/Cylinder1x2x1.hmdl");
                        }
                        else if (meshName == "Sphere")
                        {
                            writer.WriteAttributeString("file", "Assets/Mesh/Sphere1x1x1.hmdl");
                        }
                        else if (meshName == "Capsule")
                        {
                            writer.WriteAttributeString("file", "Assets/Mesh/Capsule1x2x1.hmdl");
                        }
                        else /*if (meshName == "Cube")*/
                        {
                            writer.WriteAttributeString("file", "Assets/Mesh/Cube1x1x1.hmdl");
                        }

                        writer.WriteEndElement();
                    }
                    writer.WriteEndElement();

                    if (transform.gameObject.isStatic)
                    {
                        writer.WriteStartElement("StaticCollider");
                        {
                            writer.WriteAttributeString("coll_group", "0");

                            writer.WriteStartElement("AABB");
                            {
                                writer.WriteStartElement("Min");
                                {
                                    writer.WriteAttributeString("x", renderer.bounds.min.x.ToString());
                                    writer.WriteAttributeString("y", renderer.bounds.min.y.ToString());
                                    writer.WriteAttributeString("z", renderer.bounds.min.z.ToString());
                                }
                                writer.WriteEndElement();

                                writer.WriteStartElement("Max");
                                {
                                    writer.WriteAttributeString("x", renderer.bounds.max.x.ToString());
                                    writer.WriteAttributeString("y", renderer.bounds.max.y.ToString());
                                    writer.WriteAttributeString("z", renderer.bounds.max.z.ToString());
                                }
                                writer.WriteEndElement();
                            }
                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();
                    }
                }
            }
            writer.WriteEndElement();

            if (transform.childCount != 0)
            {
                writer.WriteStartElement("Children");

                for (int i=0; i < transform.childCount; i++)
                {
                    ExportObject(writer, transform.GetChild(i));
                }

                writer.WriteEndElement();
            }
        }
        writer.WriteEndElement();
    }

    void OnGUI()
    {
        GUILayout.Label("Exporter Settings", EditorStyles.boldLabel);
        if (GUILayout.Button("Export"))
        {
            string path = EditorUtility.SaveFilePanel("Export Scene", "", "", "xml");
            Debug.Log("Saving file " + path);

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "  ";
            XmlWriter writer = XmlWriter.Create(path, settings);

            writer.WriteStartDocument();
            writer.WriteStartElement("Level");
            writer.WriteStartElement("Entities");

            // Hash set for exported prefabs
            // If any node on a prefab has been already exported, don't out put same prefab again
            HashSet<GameObject> prefabRoots = new HashSet<GameObject>();

            //foreach (GameObject obj in FindObjectsOfType<GameObject>())
            //{
            //    // PrefabUtility.GetPrefabType will return if object is a prefab
            //    PrefabType type = PrefabUtility.GetPrefabType(obj);

            //    if (type == PrefabType.PrefabInstance)
            //    {
            //        GameObject root = PrefabUtility.FindRootGameObjectWithSameParentPrefab(obj);
            //        if (root && !prefabRoots.Contains(root))
            //        {
            //            prefabRoots.Add(root);
            //            Debug.Log(root.name + ": " + type + " " + PrefabUtility.GetPrefabParent(root).name);
            //        }
            //    }
            //    else
            //        Debug.Log(obj.name + ": " + type);
            //}

            

            foreach (GameObject go in UnityEngine.SceneManagement.SceneManager.GetActiveScene().GetRootGameObjects())
            {
                ExportObject(writer, go.transform);
            }

            writer.WriteEndElement();
            writer.WriteEndElement();
            writer.WriteEndDocument();

            writer.Close();
        }
    }
}
